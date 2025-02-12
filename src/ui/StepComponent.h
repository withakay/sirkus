#pragma once

#include "../JuceHeader.h"
#include "StepButton.h"

namespace Sirkus::UI {

class StepComponent : public juce::Component, private StepButton::Listener
{
public:
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void stepButtonClicked(StepButton* button, const juce::ModifierKeys& mods) = 0;
        virtual void stepButtonStateChanged(StepButton* button) = 0;
        virtual void stepButtonRangeSelect(StepButton* button) = 0;
    };

    StepComponent();
    ~StepComponent() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

    // StepButton access
    StepButton* getStepButton()
    {
        return &stepButton;
    }

    // Listener management
    void addListener(Listener* listener);
    void removeListener(Listener* listener);

private:
    // StepButton::Listener interface
    void stepButtonClicked(StepButton* button, const juce::ModifierKeys& mods) override;
    void stepButtonStateChanged(StepButton* button) override;
    void stepButtonRangeSelect(StepButton* button) override;

    StepButton stepButton;
    juce::ListenerList<Listener> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepComponent)
};

} // namespace Sirkus::UI
