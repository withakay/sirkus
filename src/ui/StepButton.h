#pragma once

#include "../JuceHeader.h"

namespace Sirkus::UI {

class StepButton : public juce::Component {
public:
  class Listener {
  public:
    virtual ~Listener() = default;
    virtual void stepButtonClicked(StepButton *button,
                                   const juce::ModifierKeys &mods) = 0;
    virtual void stepButtonStateChanged(StepButton *button) = 0;
    virtual void stepButtonRangeSelect(StepButton *button) = 0;
  };

  StepButton();
  ~StepButton() override = default;

  void paint(juce::Graphics &) override;
  void mouseDown(const juce::MouseEvent &event) override;
  void mouseUp(const juce::MouseEvent &event) override;

  void setEnabled(bool shouldBeEnabled);
  bool isEnabled() const { return enabled; }

  void setSelected(bool shouldBeSelected);
  bool isSelected() const { return selected; }

  void setTriggered(bool shouldBeTriggered);
  bool isTriggered() const { return triggered; }

  void setZoomFactor(float factor);
  float getZoomFactor() const { return zoomFactor; }

  void addListener(Listener *listener);
  void removeListener(Listener *listener);

private:
  bool enabled{false};
  bool selected{false};
  bool triggered{false};
  float zoomFactor{1.0f};
  bool mouseWasDragged{false};

  juce::ListenerList<Listener> listeners;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepButton)
};

} // namespace Sirkus::UI
