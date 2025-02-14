#pragma once

#include "../JuceHeader.h"
#include "../core/Step.h"
#include "StepButton.h"

namespace Sirkus::UI {

class StepComponent : public juce::Component, private StepButton::Listener, private juce::Slider::Listener
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

    // Step model access
    void setStep(Core::Step* step);

    Core::Step* getStep() const
    {
        return step;
    }

    void setVelocity(float value);
    float getVelocity() const;

    // Parameter access
    void setNoteValue(float value);
    float getNoteValue() const;

    // Listener management
    void addListener(Listener* listener);
    void removeListener(Listener* listener);

private:
    // StepButton::Listener interface
    void stepButtonClicked(StepButton* button, const juce::ModifierKeys& mods) override;
    void stepButtonStateChanged(StepButton* button) override;
    void stepButtonRangeSelect(StepButton* button) override;

    // juce::Slider::Listener interface
    void sliderValueChanged(juce::Slider* slider) override;

    StepButton stepButton;
    juce::Slider noteValueSlider;
    juce::Slider velocitySlider;
    juce::Slider noteLengthSlider;
    juce::ListenerList<Listener> listeners;
    Core::Step* step{nullptr};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepComponent)
};

} // namespace Sirkus::UI
