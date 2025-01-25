#include <JuceHeader.h>

#include "PluginEditor.h"
#include "Sequencer.h"
#include "TimingManager.h"

SirkusAudioProcessorEditor::SirkusAudioProcessorEditor(SirkusAudioProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p), transportControls(p) {
  // Add and make visible all components
  addAndMakeVisible(transportControls);
  addAndMakeVisible(globalControls);
  addAndMakeVisible(patternView);
  addAndMakeVisible(stepControls);

  // Add listeners
  patternView.addListener(this);
  stepControls.addListener(this);
  globalControls.addListener(this);

  // Labels setup
  addAndMakeVisible(positionLabel);
  positionLabel.setJustificationType(juce::Justification::left);
  positionLabel.setText("Position: --", juce::dontSendNotification);

  addAndMakeVisible(bpmLabel);
  bpmLabel.setJustificationType(juce::Justification::left);
  bpmLabel.setText("BPM: --", juce::dontSendNotification);

  addAndMakeVisible(timeSignatureLabel);
  timeSignatureLabel.setJustificationType(juce::Justification::left);
  timeSignatureLabel.setText("Time Sig: --", juce::dontSendNotification);

  // Initialize UI state from processor
  updatePatternView();

  // Size setup
  setSize(900, 600);

  // Start timer for updates
  startTimerHz(30); // 30 fps update rate
}

SirkusAudioProcessorEditor::~SirkusAudioProcessorEditor()
{
    stopTimer();
    patternView.removeListener(this);
    stepControls.removeListener(this);
    globalControls.removeListener(this);
}

void SirkusAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void SirkusAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10);

    // Top section: Transport and Global Controls
    auto topSection = area.removeFromTop(70);
    transportControls.setBounds(
        topSection.removeFromLeft(topSection.getWidth() / 2));
    globalControls.setBounds(topSection);

    area.removeFromTop(10); // spacing

    // Info labels section
    auto infoSection = area.removeFromTop(25);
    positionLabel.setBounds(infoSection.removeFromLeft(200));
    bpmLabel.setBounds(infoSection.removeFromLeft(100));
    timeSignatureLabel.setBounds(infoSection.removeFromLeft(100));

    area.removeFromTop(10); // spacing

    // Pattern View (main sequencer grid)
    auto patternSection = area.removeFromTop(area.getHeight() - 100);
    patternView.setBounds(patternSection);

    area.removeFromTop(10); // spacing

    // Step Controls at bottom
    stepControls.setBounds(area);
}

void SirkusAudioProcessorEditor::timerCallback()
{
    updatePositionDisplay();
    updateTransportDisplay();
}

void SirkusAudioProcessorEditor::updatePositionDisplay()
{
    const auto& sequencer = processorRef.getSequencer();
    const auto& timing = sequencer.getTimingManager();

    if (const auto pos = timing.getMusicalPosition())
    {
        juce::String posText;
        posText << "Position: Bar " << pos->bar << " | Beat " << pos->beat << " | Tick " << static_cast<int>(pos->tick);
        positionLabel.setText(posText, juce::dontSendNotification);
    }
    else
    {
        positionLabel.setText("Position: --", juce::dontSendNotification);
    }

    if (const auto bpm = timing.getBpm())
    {
        bpmLabel.setText("BPM: " + juce::String(*bpm, 1), juce::dontSendNotification);
    }

    if (const auto timeSig = timing.getTimeSignature())
    {
        timeSignatureLabel.setText(
            "Time Sig: " + juce::String(timeSig->first) + "/" + juce::String(timeSig->second),
            juce::dontSendNotification);
    }
}

void SirkusAudioProcessorEditor::updateTransportDisplay()
{
  // Transport state is handled by TransportControls component
}

void SirkusAudioProcessorEditor::updatePatternView() {
  auto &sequencer = processorRef.getSequencer();

  // Update pattern length from first track's pattern
  if (auto *track = sequencer.getTrack(0)) {
    if (auto *pattern = track->getCurrentPattern()) {
      patternView.setPatternLength(static_cast<int>(pattern->getLength()));

      // Update step interval
      globalControls.setStepInterval(pattern->getStepInterval());
    }
  }

  // Update MIDI channels for all tracks
  for (size_t i = 0; i < sequencer.getTrackCount(); ++i) {
    if (auto *track = sequencer.getTrack(static_cast<uint32_t>(i))) {
      // Track IDs might not match index, so store mapping for future use
      SIRKUS_UNUSED(track->getId());
      SIRKUS_UNUSED(track->getMidiChannel());
      // TODO: Update track MIDI channel in UI once selection system is
      // implemented
    }
  }
}

void SirkusAudioProcessorEditor::updateSelectedSteps() {
  auto &sequencer = processorRef.getSequencer();
  const auto selectedSteps = patternView.getAllSelectedSteps();

  if (!selectedSteps.empty()) {
    // Use the first selected step to initialize the controls
    auto [trackIndex, stepIndex] = selectedSteps[0];
    if (auto *track = sequencer.getTrack(static_cast<uint32_t>(trackIndex))) {
      if (auto *pattern = track->getCurrentPattern()) {
        const auto &step = pattern->getStep(static_cast<size_t>(stepIndex));
        stepControls.setNoteValue(step.note);
        stepControls.setVelocity(step.velocity);
        stepControls.setNoteLength(step.noteLength);
      }
    }
  }
}

// PatternView::Listener implementation
void SirkusAudioProcessorEditor::patternLengthChanged(
    sirkus::ui::PatternView *view, int newLength) {
  SIRKUS_UNUSED(view); // Will be used for future view-specific operations

  auto &sequencer = processorRef.getSequencer();

  // Update all tracks' patterns to the new length
  for (size_t i = 0; i < sequencer.getTrackCount(); ++i) {
    if (auto *track = sequencer.getTrack(static_cast<uint32_t>(i))) {
      if (auto *pattern = track->getCurrentPattern()) {
        pattern->setLength(static_cast<size_t>(newLength));
      }
    }
  }
}

void SirkusAudioProcessorEditor::trackMidiChannelChanged(
    sirkus::ui::PatternView *view, int trackIndex, int newChannel) {
  SIRKUS_UNUSED(view); // Will be used for future view-specific operations

  auto &sequencer = processorRef.getSequencer();

  if (auto *track = sequencer.getTrack(static_cast<uint32_t>(trackIndex))) {
    track->setMidiChannel(static_cast<uint8_t>(newChannel));
  }
}

void SirkusAudioProcessorEditor::stepStateChanged(sirkus::ui::PatternView *view,
                                                  int trackIndex,
                                                  int stepIndex) {
  SIRKUS_UNUSED(view); // Will be used for future view-specific operations

  auto &sequencer = processorRef.getSequencer();

  if (auto *track = sequencer.getTrack(static_cast<uint32_t>(trackIndex))) {
    if (auto *pattern = track->getCurrentPattern()) {
      const auto &selectedSteps =
          patternView.getSelectedStepsForTrack(trackIndex);
      if (std::find(selectedSteps.begin(), selectedSteps.end(), stepIndex) !=
          selectedSteps.end()) {
        // Step is selected, update its state
        const auto &step = pattern->getStep(static_cast<size_t>(stepIndex));
        pattern->setStepEnabled(static_cast<size_t>(stepIndex), step.enabled);
        pattern->setStepNote(static_cast<size_t>(stepIndex), step.note);
        pattern->setStepVelocity(static_cast<size_t>(stepIndex), step.velocity);
        pattern->setStepNoteLength(static_cast<size_t>(stepIndex),
                                   step.noteLength);
      }
    }
  }
}

void SirkusAudioProcessorEditor::stepSelectionChanged(
    sirkus::ui::PatternView *view) {
  SIRKUS_UNUSED(view); // Will be used for future view-specific operations
  updateSelectedSteps();
}

// StepControls::Listener implementation
void SirkusAudioProcessorEditor::noteValueChanged(
    sirkus::ui::StepControls *controls, int newValue) {
  SIRKUS_UNUSED(controls); // Will be used for control-specific operations

  auto &sequencer = processorRef.getSequencer();
  const auto selectedSteps = patternView.getAllSelectedSteps();

  // Update note value for all selected steps
  for (const auto &[trackIndex, stepIndex] : selectedSteps) {
    if (auto *track = sequencer.getTrack(static_cast<uint32_t>(trackIndex))) {
      if (auto *pattern = track->getCurrentPattern()) {
        pattern->setStepNote(static_cast<size_t>(stepIndex),
                             static_cast<uint8_t>(newValue));
      }
    }
  }
}

void SirkusAudioProcessorEditor::velocityChanged(
    sirkus::ui::StepControls *controls, int newValue) {
  SIRKUS_UNUSED(controls); // Will be used for control-specific operations

  auto &sequencer = processorRef.getSequencer();
  const auto selectedSteps = patternView.getAllSelectedSteps();

  // Update velocity for all selected steps
  for (const auto &[trackIndex, stepIndex] : selectedSteps) {
    if (auto *track = sequencer.getTrack(static_cast<uint32_t>(trackIndex))) {
      if (auto *pattern = track->getCurrentPattern()) {
        pattern->setStepVelocity(static_cast<size_t>(stepIndex),
                                 static_cast<uint8_t>(newValue));
      }
    }
  }
}

void SirkusAudioProcessorEditor::noteLengthChanged(
    sirkus::ui::StepControls *controls, sirkus::NoteLength newLength) {
  SIRKUS_UNUSED(controls); // Will be used for control-specific operations

  auto &sequencer = processorRef.getSequencer();
  const auto selectedSteps = patternView.getAllSelectedSteps();

  // Update note length for all selected steps
  for (const auto &[trackIndex, stepIndex] : selectedSteps) {
    if (auto *track = sequencer.getTrack(static_cast<uint32_t>(trackIndex))) {
      if (auto *pattern = track->getCurrentPattern()) {
        pattern->setStepNoteLength(static_cast<size_t>(stepIndex), newLength);
      }
    }
  }
}

// GlobalControls::Listener implementation
void SirkusAudioProcessorEditor::timeSignatureChanged(
    sirkus::ui::GlobalControls *controls, int numerator, int denominator) {
  SIRKUS_UNUSED(controls); // Will be used for control-specific operations

  auto &sequencer = processorRef.getSequencer();
  auto &timing = sequencer.getTimingManager();

  timing.setTimeSignature(numerator, denominator);
}

void SirkusAudioProcessorEditor::stepIntervalChanged(
    sirkus::ui::GlobalControls *controls, sirkus::StepInterval newInterval) {
  SIRKUS_UNUSED(controls); // Will be used for control-specific operations

  auto &sequencer = processorRef.getSequencer();

  // Update step interval for all tracks' patterns
  for (size_t i = 0; i < sequencer.getTrackCount(); ++i) {
    if (auto *track = sequencer.getTrack(static_cast<uint32_t>(i))) {
      if (auto *pattern = track->getCurrentPattern()) {
        pattern->setStepInterval(newInterval);
      }
    }
  }
}
