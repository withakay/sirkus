#include "StepControls.h"
#include "../core/Types.h"

namespace Sirkus::UI {

StepControls::StepControls()
{
    setupNoteControls();
    setupVelocityControls();
    setupNoteLengthControls();
    setOpaque(true);
}

void StepControls::setupNoteControls()
{
    noteLabel = std::make_unique<juce::Label>();
    noteLabel->setText("Note", juce::dontSendNotification);
    addAndMakeVisible(noteLabel.get());

    noteSlider = std::make_unique<juce::Slider>(
        juce::Slider::LinearHorizontal,
        juce::Slider::TextBoxRight);
    noteSlider->setRange(0, 127, 1);
    noteSlider->setValue(noteValue, juce::dontSendNotification);
    noteSlider->onValueChange = [this] {
        setNoteValue(static_cast<int>(noteSlider->getValue()));
    };
    addAndMakeVisible(noteSlider.get());
}

void StepControls::setupVelocityControls()
{
    velocityLabel = std::make_unique<juce::Label>();
    velocityLabel->setText("Velocity", juce::dontSendNotification);
    addAndMakeVisible(velocityLabel.get());

    velocitySlider = std::make_unique<juce::Slider>(
        juce::Slider::LinearHorizontal,
        juce::Slider::TextBoxRight);
    velocitySlider->setRange(0, 127, 1);
    velocitySlider->setValue(velocity, juce::dontSendNotification);
    velocitySlider->onValueChange = [this] {
        setVelocity(static_cast<int>(velocitySlider->getValue()));
    };
    addAndMakeVisible(velocitySlider.get());
}

void StepControls::setupNoteLengthControls()
{
    noteLengthLabel = std::make_unique<juce::Label>();
    noteLengthLabel->setText("Length", juce::dontSendNotification);
    addAndMakeVisible(noteLengthLabel.get());

    noteLengthCombo = std::make_unique<juce::ComboBox>();
    updateNoteLengthComboBox();
    noteLengthCombo->setSelectedItemIndex(
        3,
        juce::dontSendNotification); // Default to 1/16
    noteLengthCombo->onChange = [this] {
        setNoteLength(
            static_cast<TimeDivision>(noteLengthCombo->getSelectedItemIndex()));
    };
    addAndMakeVisible(noteLengthCombo.get());
}

void StepControls::updateNoteLengthComboBox()
{
    noteLengthCombo->clear(juce::dontSendNotification);
    noteLengthCombo->addItem("1/64", TimeDivision::SixtyFourthNote);
    noteLengthCombo->addItem("1/32", TimeDivision::ThirtySecondNote);
    noteLengthCombo->addItem("1/16", TimeDivision::SixteenthNote);
    noteLengthCombo->addItem("1/8", TimeDivision::EighthNote);
    noteLengthCombo->addItem("1/4", TimeDivision::QuarterNote);
    noteLengthCombo->addItem("1/2", TimeDivision::HalfNote);
    noteLengthCombo->addItem("1", TimeDivision::WholeNote);
    noteLengthCombo->addItem("2", TimeDivision::TwoBars);
    noteLengthCombo->addItem("4", TimeDivision::FourBars);
}

void StepControls::setNoteValue(int note)
{
    note = juce::jlimit(0, 127, note);
    if (noteValue != note)
    {
        noteValue = note;
        noteSlider->setValue(noteValue, juce::dontSendNotification);
        listeners.call(
            [this](Listener& l) {
                l.noteValueChanged(this, noteValue);
            });
    }
}

void StepControls::setVelocity(int vel)
{
    vel = juce::jlimit(0, 127, vel);
    if (velocity != vel)
    {
        velocity = vel;
        velocitySlider->setValue(velocity, juce::dontSendNotification);
        listeners.call(
            [this](Listener& l) {
                l.velocityChanged(this, velocity);
            });
    }
}

void StepControls::setNoteLength(TimeDivision length)
{
    if (noteLength != length)
    {
        noteLength = length;
        noteLengthCombo->setSelectedId(
            static_cast<int>(noteLength) + 1,
            juce::dontSendNotification);
        listeners.call(
            [this](Listener& l) {
                l.noteLengthChanged(this, noteLength);
            });
    }
}

void StepControls::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void StepControls::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    const int labelWidth = 60;
    const int controlHeight = 30;
    const int spacing = 10;

    // Note controls
    noteLabel->setBounds(
        bounds.removeFromLeft(labelWidth).withHeight(controlHeight));
    bounds.removeFromLeft(spacing);
    noteSlider->setBounds(bounds.removeFromLeft(200).withHeight(controlHeight));
    bounds.removeFromLeft(spacing * 2);

    // Velocity controls
    velocityLabel->setBounds(
        bounds.removeFromLeft(labelWidth).withHeight(controlHeight));
    bounds.removeFromLeft(spacing);
    velocitySlider->setBounds(
        bounds.removeFromLeft(200).withHeight(controlHeight));
    bounds.removeFromLeft(spacing * 2);

    // Note length controls
    noteLengthLabel->setBounds(
        bounds.removeFromLeft(labelWidth).withHeight(controlHeight));
    bounds.removeFromLeft(spacing);
    noteLengthCombo->setBounds(
        bounds.removeFromLeft(100).withHeight(controlHeight));
}

void StepControls::addListener(Listener* listener)
{
    listeners.add(listener);
}

void StepControls::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

} // namespace Sirkus::UI
