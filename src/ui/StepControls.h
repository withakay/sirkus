#pragma once

#include "../Types.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace Sirkus::UI {

/**
 * @class StepControls
 * @brief Controls panel for editing step parameters.
 *
 * Provides controls for modifying note value, velocity, and note length
 * for the currently selected step(s).
 */
class StepControls : public juce::Component {
public:
  //==============================================================================
  StepControls();
  ~StepControls() override = default;

  //==============================================================================
  /** Sets the note value (0-127) */
  void setNoteValue(int note);

  /** Gets the current note value */
  int getNoteValue() const noexcept { return noteValue; }

  /** Sets the velocity (0-127) */
  void setVelocity(int vel);

  /** Gets the current velocity */
  int getVelocity() const noexcept { return velocity; }

  /** Sets the note length */
  void setNoteLength(NoteLength length);

  /** Gets the current note length */
  NoteLength getNoteLength() const noexcept { return noteLength; }

  //==============================================================================
  /** Callback interface for control changes */
  class Listener {
  public:
    virtual ~Listener() = default;
    virtual void noteValueChanged(StepControls *controls, int newValue) = 0;
    virtual void velocityChanged(StepControls *controls, int newValue) = 0;
    virtual void noteLengthChanged(StepControls *controls,
                                   NoteLength newLength) = 0;
  };

  void addListener(Listener *listener);
  void removeListener(Listener *listener);

  //==============================================================================
  /** @internal */
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  //==============================================================================
  std::unique_ptr<juce::Slider> noteSlider;
  std::unique_ptr<juce::Label> noteLabel;
  std::unique_ptr<juce::Slider> velocitySlider;
  std::unique_ptr<juce::Label> velocityLabel;
  std::unique_ptr<juce::ComboBox> noteLengthCombo;
  std::unique_ptr<juce::Label> noteLengthLabel;

  int noteValue = 60; // Middle C
  int velocity = 100;
  NoteLength noteLength = NoteLength::Sixteen;

  juce::ListenerList<Listener> listeners;

  void setupNoteControls();
  void setupVelocityControls();
  void setupNoteLengthControls();
  void updateNoteLengthComboBox();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepControls)
};

} // namespace Sirkus::UI
