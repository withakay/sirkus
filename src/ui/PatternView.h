#pragma once

#include "StepTrack.h"
#include <JuceHeader.h>

namespace sirkus::ui {

class PatternView : public juce::Component, private StepTrack::Listener {
public:
  class Listener {
  public:
    virtual ~Listener() = default;
    virtual void patternLengthChanged(PatternView *view, int newLength) = 0;
    virtual void trackMidiChannelChanged(PatternView *view, int trackIndex,
                                         int newChannel) = 0;
    virtual void stepStateChanged(PatternView *view, int trackIndex,
                                  int stepIndex) = 0;
    virtual void stepSelectionChanged(PatternView *view) = 0;
  };

  static constexpr int NUM_TRACKS = 4;
  static constexpr int STEPS_PER_PAGE = 16;
  static constexpr float MIN_ZOOM = 0.5f;
  static constexpr float MAX_ZOOM = 2.0f;

  PatternView();
  ~PatternView() override;

  void paint(juce::Graphics &) override;
  void resized() override;

  void setPatternLength(int numSteps);
  void setCurrentPage(int newPage);
  void setZoomLevel(float level);

  // Selection methods
  void clearAllStepSelections();
  const std::vector<int> &getSelectedStepsForTrack(int trackIndex) const;
  std::vector<std::pair<int, int>> getAllSelectedSteps() const;

  // Trigger methods
  void clearAllTriggers();
  void setStepTriggered(int trackIndex, int stepIndex, bool triggered);

  void addListener(Listener *listener);
  void removeListener(Listener *listener);

private:
  // StepTrack::Listener implementation
  void trackMidiChannelChanged(StepTrack *track, int newChannel) override;
  void stepStateChanged(StepTrack *track, int stepIndex) override;

  void updatePageControls();
  void updatePatternLength();
  int getNumPages() const;
  int getTrackIndex(StepTrack *track) const;

  // Pattern controls
  std::unique_ptr<juce::Label> patternLengthLabel;
  std::unique_ptr<juce::Slider> patternLengthSlider;

  // Page navigation
  std::unique_ptr<juce::TextButton> prevPageButton;
  std::unique_ptr<juce::TextButton> nextPageButton;
  std::unique_ptr<juce::Label> pageLabel;

  // Zoom control
  std::unique_ptr<juce::Slider> zoomSlider;

  // Pattern state
  int patternLength{16};
  int currentPage{0};
  float zoomLevel{1.0f};

  // Tracks
  std::vector<std::unique_ptr<StepTrack>> tracks;

  juce::ListenerList<Listener> listeners;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternView)
};

} // namespace sirkus::ui