#pragma once

#include "../Types.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace sirkus::ui {

/**
 * @class StepButton
 * @brief A button component representing a single step in a pattern.
 *
 * Displays and allows editing of step properties including on/off state,
 * trigger status, note value, velocity, and note length.
 */
class StepButton : public juce::Component {
public:
  //==============================================================================
  StepButton();
  ~StepButton() override = default;

  //==============================================================================
  /** Sets whether the step is enabled (will trigger) */
  void setEnabled(bool shouldBeEnabled);

  /** Sets whether the step is currently being triggered */
  void setTriggered(bool isTriggered);

  /** Sets the note value for this step */
  void setNoteValue(int note);

  /** Sets the velocity for this step */
  void setVelocity(int velocity);

  /** Sets the note length for this step */
  void setNoteLength(NoteLength length);

  /** Sets whether this step is selected */
  void setSelected(bool shouldBeSelected);

  /** Sets the zoom scale factor for the button */
  void setZoomFactor(float factor);

  //==============================================================================
  /** Returns whether the step is enabled */
  bool isEnabled() const noexcept { return enabled; }

  /** Returns whether the step is currently triggered */
  bool isTriggered() const noexcept { return triggered; }

  /** Returns the current note value */
  int getNoteValue() const noexcept { return noteValue; }

  /** Returns the current velocity */
  int getVelocity() const noexcept { return velocity; }

  /** Returns the current note length */
  NoteLength getNoteLength() const noexcept { return noteLength; }

  /** Returns whether this step is selected */
  bool isSelected() const noexcept { return selected; }

  //==============================================================================
  /** @internal */
  void paint(juce::Graphics &g) override;
  void resized() override;
  void mouseDown(const juce::MouseEvent &e) override;
  void mouseUp(const juce::MouseEvent &e) override;

  //==============================================================================
  /** Callback interface for step button events */
  class Listener {
  public:
    virtual ~Listener() = default;

    /** Called when the button is clicked with various modifiers */
    virtual void stepButtonClicked(StepButton *button,
                                   const juce::ModifierKeys &mods) = 0;

    /** Called when the button's state changes (enabled, note, velocity, etc.)
     */
    virtual void stepButtonStateChanged(StepButton *button) = 0;

    /** Called when the button is clicked with shift for range selection */
    virtual void stepButtonRangeSelect(StepButton *button) = 0;
  };

  void addListener(Listener *listener);
  void removeListener(Listener *listener);

private:
  //==============================================================================
  bool enabled = false;
  bool triggered = false;
  bool selected = false;
  int noteValue = 60; // Middle C
  int velocity = 100; // Default velocity
  NoteLength noteLength = NoteLength::Sixteen;
  float zoomFactor = 1.0f;

  juce::ListenerList<Listener> listeners;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepButton)
};

} // namespace sirkus::ui
