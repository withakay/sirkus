#include "StepButton.h"

namespace Sirkus::UI {

StepButton::StepButton()
{
    setOpaque(true);
}

void StepButton::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();

    // Background
    g.setColour(enabled ? juce::Colours::darkgreen : juce::Colour::fromRGB(40, 40, 40));
    g.fillRect(bounds);

    // Selection highlight
    if (selected)
    {
        if (enabled)
            g.setColour(juce::Colours::green);
        else
            g.setColour(juce::Colour::fromRGB(40, 40, 40));

        g.fillRect(bounds);
    }

    // Trigger indicator
    if (triggered)
    {
        g.setColour(juce::Colours::red);
        const auto diameter = 4.0f;
        const auto centeredBounds = bounds.withSizeKeepingCentre(diameter, diameter);
        g.fillEllipse(centeredBounds);
    }

    // Border
    g.setColour(juce::Colours::black);
    if (selected)
        g.drawRect(bounds, 2.0f);
    else
        g.drawRect(bounds, 1.0f);
}

void StepButton::mouseDown(const juce::MouseEvent& event)
{
    mouseWasDragged = false;

    if (event.mods.isShiftDown())
    {
        listeners.call(
            [this](Listener& l) {
                l.stepButtonRangeSelect(this);
            });
    }
    else
    {
        const auto mods = event.mods; // Capture the mods locally
        listeners.call(
            [this, mods](Listener& l) {
                l.stepButtonClicked(this, mods);
            });

        // Only notify state change if enabled state actually changes
        const bool newEnabled = !enabled;
        if (enabled != newEnabled)
        {
            enabled = newEnabled;
            repaint();
            listeners.call(
                [this](Listener& l) {
                    l.stepButtonStateChanged(this);
                });
        }
    }
}

void StepButton::mouseUp(const juce::MouseEvent& event)
{
    if (mouseWasDragged)
    {
        listeners.call(
            [this](Listener& l) {
                l.stepButtonStateChanged(this);
            });
    }
    mouseWasDragged = false;
}

void StepButton::setEnabled(bool shouldBeEnabled)
{
    if (enabled != shouldBeEnabled)
    {
        enabled = shouldBeEnabled;
        repaint();
        listeners.call(
            [this](Listener& l) {
                l.stepButtonStateChanged(this);
            });
    }
}

void StepButton::setSelected(bool shouldBeSelected)
{
    if (selected != shouldBeSelected)
    {
        selected = shouldBeSelected;
        repaint();
    }
}

void StepButton::setTriggered(bool shouldBeTriggered)
{
    if (triggered != shouldBeTriggered)
    {
        triggered = shouldBeTriggered;
        repaint();
    }
}

void StepButton::setZoomFactor(float factor)
{
    if (std::abs(zoomFactor - factor) > 0.0001f)
    {
        zoomFactor = factor;
        repaint();
    }
}

void StepButton::addListener(Listener* listener)
{
    listeners.add(listener);
}

void StepButton::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

} // namespace Sirkus::UI
