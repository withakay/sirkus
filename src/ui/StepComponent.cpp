#include "StepComponent.h"

namespace Sirkus::UI {

StepComponent::StepComponent()
{
    setOpaque(true);
    addAndMakeVisible(stepButton);
    stepButton.addListener(this);
}

void StepComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void StepComponent::resized()
{
    stepButton.setBounds(getLocalBounds());
}

void StepComponent::addListener(Listener* listener)
{
    listeners.add(listener);
}

void StepComponent::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

void StepComponent::stepButtonClicked(StepButton* button, const juce::ModifierKeys& mods)
{
    listeners.call([button, &mods](Listener& l) { l.stepButtonClicked(button, mods); });
}

void StepComponent::stepButtonStateChanged(StepButton* button)
{
    listeners.call([button](Listener& l) { l.stepButtonStateChanged(button); });
}

void StepComponent::stepButtonRangeSelect(StepButton* button)
{
    listeners.call([button](Listener& l) { l.stepButtonRangeSelect(button); });
}

} // namespace Sirkus::UI
