#pragma once

#include "../Constants.h"
#include "../Types.h"
#include "StepTrack.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace sirkus::ui {

/**
 * @class PatternView
 * @brief Main view component that manages multiple tracks and pattern-wide
 * controls.
 *
 * Handles the layout of tracks, pattern length control, paging, and zoom
 * functionality. Initially supports 4 tracks but designed for future expansion.
 */
class PatternView : public juce::Component, private StepTrack::Listener {
public:
  //==============================================================================
  PatternView();
  ~PatternView() override;

  //==============================================================================
  /** Sets the pattern length in steps */
  void setPatternLength(int numSteps);

  /** Gets the current pattern length */
  int getPatternLength() const noexcept { return patternLength; }

  /** Sets the current page */
  void setCurrentPage(int newPage);

  /** Gets the current page */
  int getCurrentPage() const noexcept { return currentPage; }

  /** Sets the zoom level (0.5 - 2.0) */
  void setZoomLevel(float level);

  /** Gets the current zoom level */
  float getZoomLevel() const noexcept { return zoomLevel; }

  /** Clears all step selections across all tracks */
  void clearAllStepSelections();

  /** Gets the currently selected steps for a track */
  const std::vector<int> &getSelectedStepsForTrack(int trackIndex) const;

  /** Gets all selected steps across all tracks */
  std::vector<std::pair<int, int>>
  getAllSelectedSteps() const; // Returns pairs of (trackIndex, stepIndex)

  //==============================================================================
  /** Callback interface for pattern events */
  class Listener {
  public:
    virtual ~Listener() = default;
    virtual void patternLengthChanged(PatternView *view, int newLength) = 0;
    virtual void trackMidiChannelChanged(PatternView *view, int trackIndex,
                                         int newChannel) = 0;
    virtual void stepStateChanged(PatternView *view, int trackIndex,
                                  int stepIndex) = 0;
    virtual void stepSelectionChanged(
        PatternView *view) = 0; // Called when step selection changes
  };

  void addListener(Listener *listener);
  void removeListener(Listener *listener);

  //==============================================================================
  /** @internal */
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  //==============================================================================
  /** StepTrack::Listener implementation */
  void trackMidiChannelChanged(StepTrack *track, int newChannel) override;
  void stepStateChanged(StepTrack *track, int stepIndex) override;

  std::vector<std::unique_ptr<StepTrack>> tracks;
  std::unique_ptr<juce::Slider> patternLengthSlider;
  std::unique_ptr<juce::Label> patternLengthLabel;
  std::unique_ptr<juce::TextButton> prevPageButton;
  std::unique_ptr<juce::TextButton> nextPageButton;
  std::unique_ptr<juce::Label> pageLabel;
  std::unique_ptr<juce::Slider> zoomSlider;

  int patternLength = 16;
  int currentPage = 0;
  float zoomLevel = 1.0f;

  static constexpr int NUM_TRACKS = 4;
  static constexpr int STEPS_PER_PAGE = 16;
  static constexpr float MIN_ZOOM = 0.5f;
  static constexpr float MAX_ZOOM = 2.0f;

  juce::ListenerList<Listener> listeners;

  void updatePageControls();
  void updatePatternLength();
  int getNumPages() const;
  int getTrackIndex(StepTrack *track) const;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternView)
};

} // namespace sirkus::ui