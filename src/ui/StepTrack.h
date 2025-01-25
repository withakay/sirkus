#pragma once

#include "../Constants.h"
#include "../Types.h"
#include "StepButton.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace sirkus::ui {

/**
 * @class StepTrack
 * @brief A component that represents a single track in the sequencer.
 *
 * Contains a row of StepButtons and track-specific controls like MIDI channel
 * selection. Handles paging for patterns longer than the visible width.
 */
class StepTrack : public juce::Component, private StepButton::Listener {
public:
  //==============================================================================
  StepTrack();
  ~StepTrack() override;

  //==============================================================================
  /** Sets the MIDI channel for this track (0-15, 0 = omni) */
  void setMidiChannel(int channel);

  /** Gets the current MIDI channel */
  int getMidiChannel() const noexcept { return midiChannel; }

  /** Sets the current page (for patterns > 16 steps) */
  void setCurrentPage(int newPage);

  /** Gets the current page */
  int getCurrentPage() const noexcept { return currentPage; }

  /** Sets the total number of pages */
  void setTotalPages(int numPages);

  /** Gets the total number of pages */
  int getTotalPages() const noexcept { return totalPages; }

  /** Sets the track number/ID */
  void setTrackNumber(int number);

  /** Gets the track number/ID */
  int getTrackNumber() const noexcept { return trackNumber; }

  /** Sets the zoom factor for all step buttons */
  void setZoomFactor(float factor);

  /** Clears all step selections */
  void clearStepSelection();

  /** Gets the currently selected step indices */
  const std::vector<int> &getSelectedStepIndices() const {
    return selectedStepIndices;
  }

  //==============================================================================
  /** Callback interface for track events */
  class Listener {
  public:
    virtual ~Listener() = default;
    virtual void trackMidiChannelChanged(StepTrack *track, int newChannel) = 0;
    virtual void stepStateChanged(StepTrack *track, int stepIndex) = 0;
  };

  void addListener(Listener *listener);
  void removeListener(Listener *listener);

  //==============================================================================
  /** @internal */
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  //==============================================================================
  /** StepButton::Listener implementation */
  void stepButtonClicked(StepButton *button,
                         const juce::ModifierKeys &mods) override;
  void stepButtonStateChanged(StepButton *button) override;
  void stepButtonRangeSelect(StepButton *button) override;

  std::vector<std::unique_ptr<StepButton>> stepButtons;
  std::unique_ptr<juce::ComboBox> midiChannelSelector;
  std::unique_ptr<juce::Label> trackLabel;

  int midiChannel = 0; // 0 = omni
  int currentPage = 0;
  int totalPages = 1;
  int trackNumber = 0;
  float zoomFactor = 1.0f;

  std::vector<int> selectedStepIndices;
  int lastSelectedStepIndex = -1; // For range selection

  static constexpr int VISIBLE_STEPS = 16;

  juce::ListenerList<Listener> listeners;

  void handleStepSelection(StepButton *button, const juce::ModifierKeys &mods);
  void selectStepRange(int fromIndex, int toIndex);
  int getStepButtonIndex(StepButton *button) const;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepTrack)
};

} // namespace sirkus::ui