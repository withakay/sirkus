#include "PatternView.h"

namespace sirkus::ui {

PatternView::PatternView() {
  // Create pattern length controls
  patternLengthLabel = std::make_unique<juce::Label>();
  patternLengthLabel->setText("Pattern Length", juce::dontSendNotification);
  addAndMakeVisible(patternLengthLabel.get());

  patternLengthSlider = std::make_unique<juce::Slider>(
      juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight);
  patternLengthSlider->setRange(1, MAX_STEPS, 1);
  patternLengthSlider->setValue(patternLength, juce::dontSendNotification);
  patternLengthSlider->onValueChange = [this] {
    setPatternLength(static_cast<int>(patternLengthSlider->getValue()));
  };
  addAndMakeVisible(patternLengthSlider.get());

  // Create page navigation controls
  prevPageButton = std::make_unique<juce::TextButton>("←");
  prevPageButton->onClick = [this] { setCurrentPage(currentPage - 1); };
  addAndMakeVisible(prevPageButton.get());

  nextPageButton = std::make_unique<juce::TextButton>("→");
  nextPageButton->onClick = [this] { setCurrentPage(currentPage + 1); };
  addAndMakeVisible(nextPageButton.get());

  pageLabel = std::make_unique<juce::Label>();
  pageLabel->setJustificationType(juce::Justification::centred);
  addAndMakeVisible(pageLabel.get());

  // Create zoom control
  zoomSlider = std::make_unique<juce::Slider>(juce::Slider::LinearHorizontal,
                                              juce::Slider::TextBoxRight);
  zoomSlider->setRange(MIN_ZOOM, MAX_ZOOM, 0.1);
  zoomSlider->setValue(zoomLevel, juce::dontSendNotification);
  zoomSlider->onValueChange = [this] {
    setZoomLevel(static_cast<float>(zoomSlider->getValue()));
  };
  addAndMakeVisible(zoomSlider.get());

  // Create tracks
  for (int i = 0; i < NUM_TRACKS; ++i) {
    auto track = std::make_unique<StepTrack>();
    track->addListener(this);
    track->setTrackNumber(i);
    addAndMakeVisible(track.get());
    tracks.push_back(std::move(track));
  }

  updatePageControls();
  setOpaque(true);
}

PatternView::~PatternView() {
  for (auto &track : tracks)
    track->removeListener(this);
}

void PatternView::setPatternLength(int numSteps) {
  numSteps = juce::jlimit(1, MAX_STEPS, numSteps);
  if (patternLength != numSteps) {
    patternLength = numSteps;
    updatePatternLength();
    listeners.call(
        [this](Listener &l) { l.patternLengthChanged(this, patternLength); });
  }
}

void PatternView::setCurrentPage(int newPage) {
  int numPages = getNumPages();
  newPage = juce::jlimit(0, numPages - 1, newPage);

  if (currentPage != newPage) {
    currentPage = newPage;
    for (auto &track : tracks)
      track->setCurrentPage(currentPage);
    updatePageControls();
  }
}

void PatternView::setZoomLevel(float level) {
  level = juce::jlimit(MIN_ZOOM, MAX_ZOOM, level);

  static constexpr float epsilon = 0.0001f;
  if (std::abs(zoomLevel - level) > epsilon) {
    zoomLevel = level;
    for (auto &track : tracks)
      track->setZoomFactor(zoomLevel);
    resized();
  }
}

void PatternView::clearAllStepSelections() {
  for (auto &track : tracks)
    track->clearStepSelection();

  listeners.call([this](Listener &l) { l.stepSelectionChanged(this); });
}

const std::vector<int> &
PatternView::getSelectedStepsForTrack(int trackIndex) const {
  static const std::vector<int> emptySelection;

  if (trackIndex >= 0 && static_cast<size_t>(trackIndex) < tracks.size())
    return tracks[static_cast<size_t>(trackIndex)]->getSelectedStepIndices();

  return emptySelection;
}

std::vector<std::pair<int, int>> PatternView::getAllSelectedSteps() const {
  std::vector<std::pair<int, int>> allSelected;

  for (size_t trackIndex = 0; trackIndex < tracks.size(); ++trackIndex) {
    const auto &trackSelection = tracks[trackIndex]->getSelectedStepIndices();
    for (int stepIndex : trackSelection) {
      allSelected.emplace_back(static_cast<int>(trackIndex), stepIndex);
    }
  }

  return allSelected;
}

void PatternView::paint(juce::Graphics &g) { g.fillAll(juce::Colours::black); }

void PatternView::resized() {
  auto bounds = getLocalBounds();

  // Layout controls at top
  auto topControls = bounds.removeFromTop(40);

  patternLengthLabel->setBounds(topControls.removeFromLeft(100));
  patternLengthSlider->setBounds(topControls.removeFromLeft(200));

  auto pageControls = topControls.removeFromLeft(200);
  prevPageButton->setBounds(pageControls.removeFromLeft(40));
  nextPageButton->setBounds(pageControls.removeFromRight(40));
  pageLabel->setBounds(pageControls);

  zoomSlider->setBounds(topControls.removeFromLeft(150));

  // Layout tracks
  bounds.removeFromTop(10); // Spacing
  const int trackHeight =
      (bounds.getHeight() - ((NUM_TRACKS - 1) * 10)) / NUM_TRACKS;

  for (auto &track : tracks) {
    track->setBounds(bounds.removeFromTop(trackHeight));
    bounds.removeFromTop(10); // Spacing between tracks
  }
}

void PatternView::trackMidiChannelChanged(StepTrack *track, int newChannel) {
  if (auto trackIndex = getTrackIndex(track); trackIndex >= 0) {
    listeners.call([this, trackIndex, newChannel](Listener &l) {
      l.trackMidiChannelChanged(this, trackIndex, newChannel);
    });
  }
}

void PatternView::stepStateChanged(StepTrack *track, int stepIndex) {
  if (auto trackIndex = getTrackIndex(track); trackIndex >= 0) {
    listeners.call([this, trackIndex, stepIndex](Listener &l) {
      l.stepStateChanged(this, trackIndex, stepIndex);
    });

    // Selection may have changed
    listeners.call([this](Listener &l) { l.stepSelectionChanged(this); });
  }
}

void PatternView::addListener(Listener *listener) { listeners.add(listener); }

void PatternView::removeListener(Listener *listener) {
  listeners.remove(listener);
}

void PatternView::updatePageControls() {
  int numPages = getNumPages();
  prevPageButton->setEnabled(currentPage > 0);
  nextPageButton->setEnabled(currentPage < numPages - 1);
  pageLabel->setText(juce::String(currentPage + 1) + "/" +
                         juce::String(numPages),
                     juce::dontSendNotification);
}

void PatternView::updatePatternLength() {
  int numPages = getNumPages();
  currentPage = juce::jmin(currentPage, numPages - 1);

  for (auto &track : tracks) {
    track->setTotalPages(numPages);
    track->setCurrentPage(currentPage);
  }

  updatePageControls();
}

int PatternView::getNumPages() const {
  return (patternLength + STEPS_PER_PAGE - 1) / STEPS_PER_PAGE;
}

int PatternView::getTrackIndex(StepTrack *track) const {
  auto it =
      std::find_if(tracks.begin(), tracks.end(),
                   [track](const auto &ptr) { return ptr.get() == track; });

  return it != tracks.end()
             ? static_cast<int>(std::distance(tracks.begin(), it))
             : -1;
}

} // namespace sirkus::ui