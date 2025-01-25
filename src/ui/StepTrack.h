#pragma once

#include "StepButton.h"
#include <JuceHeader.h>
#include <vector>

namespace sirkus::ui {

class StepTrack : public juce::Component, private StepButton::Listener {
public:
  class Listener {
  public:
    virtual ~Listener() = default;
    virtual void trackMidiChannelChanged(StepTrack *track, int newChannel) = 0;
    virtual void stepStateChanged(StepTrack *track, int stepIndex) = 0;
  };

  static constexpr int VISIBLE_STEPS = 16;

  StepTrack();
  ~StepTrack() override;

  void paint(juce::Graphics &) override;
  void resized() override;

  // Track settings
  void setMidiChannel(int channel);
  void setTrackNumber(int number);
  void setZoomFactor(float factor);

  // Page management
  void setCurrentPage(int newPage);
  void setTotalPages(int numPages);

  // Selection methods
  void clearStepSelection();
  const std::vector<int> &getSelectedStepIndices() const {
    return selectedStepIndices;
  }

    // Trigger methods
    void clearAllTriggers();
    void setStepTriggered(int stepIndex, bool triggered);

    void addListener(Listener *listener);
    void removeListener(Listener *listener);

  private:
  // StepButton::Listener implementation
  void stepButtonClicked(StepButton *button,
                         const juce::ModifierKeys &mods) override;
  void stepButtonStateChanged(StepButton *button) override;
  void stepButtonRangeSelect(StepButton *button) override;

  void handleStepSelection(StepButton *button, const juce::ModifierKeys &mods);
  void selectStepRange(int fromIndex, int toIndex);
  int getStepButtonIndex(StepButton *button) const;

  std::unique_ptr<juce::ComboBox> midiChannelSelector;
  std::unique_ptr<juce::Label> trackLabel;
  std::vector<std::unique_ptr<StepButton>> stepButtons;
  std::vector<int> selectedStepIndices;

  int midiChannel{-1}; // -1 = omni
  int trackNumber{0};
  int currentPage{0};
  int totalPages{1};
  float zoomFactor{1.0f};
  int lastSelectedStepIndex{-1};

  juce::ListenerList<Listener> listeners;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepTrack)
};

} // namespace sirkus::ui