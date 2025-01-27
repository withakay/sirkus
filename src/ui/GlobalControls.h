#pragma once

#include "../Types.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace Sirkus::UI {

/**
 * @class GlobalControls
 * @brief Controls panel for global sequencer settings.
 *
 * Provides controls for time signature, step interval, and other
 * global sequencer parameters.
 */
class GlobalControls : public juce::Component {
public:
  //==============================================================================
  GlobalControls();
  ~GlobalControls() override = default;

  //==============================================================================
  /** Sets the time signature numerator */
  void setTimeSignatureNumerator(int numerator);

  /** Gets the time signature numerator */
  int getTimeSignatureNumerator() const noexcept { return timeSigNumerator; }

  /** Sets the time signature denominator */
  void setTimeSignatureDenominator(int denominator);

  /** Gets the time signature denominator */
  int getTimeSignatureDenominator() const noexcept {
    return timeSigDenominator;
  }

  /** Sets the step interval */
  void setStepInterval(StepInterval interval);

  /** Gets the current step interval */
  StepInterval getStepInterval() const noexcept { return stepInterval; }

  //==============================================================================
  /** Callback interface for global control changes */
  class Listener {
  public:
    virtual ~Listener() = default;
    virtual void timeSignatureChanged(GlobalControls *controls, int numerator,
                                      int denominator) = 0;
    virtual void stepIntervalChanged(GlobalControls *controls,
                                     StepInterval newInterval) = 0;
  };

  void addListener(Listener *listener);
  void removeListener(Listener *listener);

  //==============================================================================
  /** @internal */
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  //==============================================================================
  std::unique_ptr<juce::ComboBox> timeSigNumeratorCombo;
  std::unique_ptr<juce::ComboBox> timeSigDenominatorCombo;
  std::unique_ptr<juce::Label> timeSigLabel;
  std::unique_ptr<juce::ComboBox> stepIntervalCombo;
  std::unique_ptr<juce::Label> stepIntervalLabel;

  int timeSigNumerator = 4;
  int timeSigDenominator = 4;
  StepInterval stepInterval = StepInterval::Sixteen;

  juce::ListenerList<Listener> listeners;

  void setupTimeSignatureControls();
  void setupStepIntervalControls();
  void updateTimeSignatureCombos();
  void updateStepIntervalComboBox();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GlobalControls)
};

} // namespace Sirkus::UI
