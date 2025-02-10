#include "GlobalControls.h"

namespace Sirkus::UI {

using namespace Core;

GlobalControls::GlobalControls()
{
    setupTimeSignatureControls();
    setupStepIntervalControls();
    setOpaque(true);
}

void GlobalControls::setupTimeSignatureControls()
{
    timeSigLabel = std::make_unique<juce::Label>();
    timeSigLabel->setText("Time Signature", juce::dontSendNotification);
    addAndMakeVisible(timeSigLabel.get());

    timeSigNumeratorCombo = std::make_unique<juce::ComboBox>();
    timeSigDenominatorCombo = std::make_unique<juce::ComboBox>();
    updateTimeSignatureCombos();

    timeSigNumeratorCombo->setSelectedId(
        timeSigNumerator,
        juce::dontSendNotification);
    timeSigNumeratorCombo->onChange = [this] {
        setTimeSignatureNumerator(timeSigNumeratorCombo->getSelectedId());
    };

    timeSigDenominatorCombo->setSelectedId(
        timeSigDenominator,
        juce::dontSendNotification);
    timeSigDenominatorCombo->onChange = [this] {
        setTimeSignatureDenominator(timeSigDenominatorCombo->getSelectedId());
    };

    addAndMakeVisible(timeSigNumeratorCombo.get());
    addAndMakeVisible(timeSigDenominatorCombo.get());
}

void GlobalControls::setupStepIntervalControls()
{
    stepIntervalLabel = std::make_unique<juce::Label>();
    stepIntervalLabel->setText("Step Interval", juce::dontSendNotification);
    addAndMakeVisible(stepIntervalLabel.get());

    stepIntervalCombo = std::make_unique<juce::ComboBox>();
    updateStepIntervalComboBox();
    stepIntervalCombo->setSelectedItemIndex(
        2,
        juce::dontSendNotification); // Default to 1/16
    stepIntervalCombo->onChange = [this] {
        setStepInterval(
            static_cast<TimeDivision>(stepIntervalCombo->getSelectedItemIndex()));
    };
    addAndMakeVisible(stepIntervalCombo.get());
}

void GlobalControls::updateTimeSignatureCombos()
{
    // Numerator options (1-16)
    timeSigNumeratorCombo->clear(juce::dontSendNotification);
    for (int i = 1; i <= 16; ++i)
        timeSigNumeratorCombo->addItem(juce::String(i), i);

    // Denominator options (powers of 2: 1,2,4,8,16,32)
    timeSigDenominatorCombo->clear(juce::dontSendNotification);
    for (int i = 0; i <= 5; ++i)
    {
        int denominator = 1 << i;
        timeSigDenominatorCombo->addItem(juce::String(denominator), denominator);
    }
}

void GlobalControls::updateStepIntervalComboBox()
{
    stepIntervalCombo->clear(juce::dontSendNotification);
    stepIntervalCombo->addItem("1/64",TimeDivision::SixtyFourthNote);
    stepIntervalCombo->addItem("1/32",TimeDivision::ThirtySecondNote);
    stepIntervalCombo->addItem("1/16",TimeDivision::SixteenthNote);
    stepIntervalCombo->addItem("1/8", TimeDivision::EighthNote);
    stepIntervalCombo->addItem("1/4",TimeDivision::QuarterNote);
}

void GlobalControls::setTimeSignatureNumerator(int numerator)
{
    if (timeSigNumerator != numerator)
    {
        timeSigNumerator = numerator;
        timeSigNumeratorCombo->setSelectedId(numerator, juce::dontSendNotification);
        listeners.call(
            [this](Listener& l) {
                l.timeSignatureChanged(this, timeSigNumerator, timeSigDenominator);
            });
    }
}

void GlobalControls::setTimeSignatureDenominator(int denominator)
{
    if (timeSigDenominator != denominator)
    {
        timeSigDenominator = denominator;
        timeSigDenominatorCombo->setSelectedId(
            denominator,
            juce::dontSendNotification);
        listeners.call(
            [this](Listener& l) {
                l.timeSignatureChanged(this, timeSigNumerator, timeSigDenominator);
            });
    }
}

void GlobalControls::setStepInterval(TimeDivision interval)
{
    if (stepInterval != interval)
    {
        stepInterval = interval;
        stepIntervalCombo->setSelectedId(
            static_cast<int>(interval) + 1,
            juce::dontSendNotification);
        listeners.call(
            [this](Listener& l) {
                l.stepIntervalChanged(this, stepInterval);
            });
    }
}

void GlobalControls::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void GlobalControls::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    const int labelWidth = 100;
    const int controlHeight = 30;
    const int spacing = 10;
    const int comboWidth = 60;

    // Time signature controls
    timeSigLabel->setBounds(
        bounds.removeFromLeft(labelWidth).withHeight(controlHeight));
    bounds.removeFromLeft(spacing);

    auto timeSigBounds =
        bounds.removeFromLeft(comboWidth * 2 + 20).withHeight(controlHeight);
    timeSigNumeratorCombo->setBounds(timeSigBounds.removeFromLeft(comboWidth));
    timeSigBounds.removeFromLeft(20); // Space for "/"
    timeSigDenominatorCombo->setBounds(timeSigBounds);

    bounds.removeFromLeft(spacing * 2);

    // Step interval controls
    stepIntervalLabel->setBounds(
        bounds.removeFromLeft(labelWidth).withHeight(controlHeight));
    bounds.removeFromLeft(spacing);
    stepIntervalCombo->setBounds(
        bounds.removeFromLeft(100).withHeight(controlHeight));
}

void GlobalControls::addListener(Listener* listener)
{
    listeners.add(listener);
}

void GlobalControls::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

} // namespace Sirkus::UI
