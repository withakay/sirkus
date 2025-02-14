#include "StepComponent.h"

namespace Sirkus::UI {

StepComponent::StepComponent()
{
    // Component setup
    setOpaque(true);

    // Step button setup
    addAndMakeVisible(stepButton);
    stepButton.addListener(this);

    // Note value rotary slider setup
    addAndMakeVisible(noteValueSlider);
    noteValueSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    noteValueSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    noteValueSlider.addListener(this);
    noteValueSlider.setRange(0.0, 127.0, 1.0);
    noteValueSlider.setValue(60.0, juce::dontSendNotification);
    noteValueSlider.setDoubleClickReturnValue(true, 60.0);

    // Velocity slider setup
    addAndMakeVisible(velocitySlider);
    velocitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    velocitySlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    velocitySlider.addListener(this);
    velocitySlider.setRange(0.0, 127.0, 1.0);
    velocitySlider.setValue(100.0, juce::dontSendNotification);
    velocitySlider.setDoubleClickReturnValue(true, 100.0);

    // Note length slider setup
    addAndMakeVisible(noteLengthSlider);
    noteLengthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    noteLengthSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    noteLengthSlider.addListener(this);

    // Set up note length options (focusing on common note lengths)
    const int numNoteLengths = 10;
    noteLengthSlider.setRange(0.0, numNoteLengths - 1, 1.0);
    noteLengthSlider.setValue(static_cast<double>(Core::TimeDivision::SixteenthNote), juce::dontSendNotification);
    noteLengthSlider.setDoubleClickReturnValue(true, static_cast<double>(Core::TimeDivision::SixteenthNote));
}

void StepComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void StepComponent::resized()
{
    auto bounds = getLocalBounds();

    // Reserve top portion for rotary slider (1/3 of height)
    auto rotaryHeight = bounds.getHeight() / 3;
    noteValueSlider.setBounds(bounds.removeFromTop(rotaryHeight));

    // Reserve space for horizontal sliders (1/6 of height each)
    auto horizontalSliderHeight = rotaryHeight / 2;
    velocitySlider.setBounds(bounds.removeFromTop(horizontalSliderHeight));
    noteLengthSlider.setBounds(bounds.removeFromTop(horizontalSliderHeight));

    // Use remaining space for step button (1/3 of height)
    jassert(bounds.getHeight() == rotaryHeight);
    stepButton.setBounds(bounds);
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

void StepComponent::sliderValueChanged(juce::Slider* slider)
{
    if (step != nullptr)
    {
        if (slider == &noteValueSlider)
        {
            step->setNote(static_cast<uint8_t>(slider->getValue()));
        }
        else if (slider == &velocitySlider)
        {
            step->setVelocity(static_cast<uint8_t>(slider->getValue()));
        }
        else if (slider == &noteLengthSlider)
        {
            // Convert slider value to TimeDivision
            Core::TimeDivision length;
            switch (static_cast<int>(slider->getValue()))
            {
                case 0:
                    length = Core::TimeDivision::SixteenthNote;
                    break;
                case 1:
                    length = Core::TimeDivision::DottedSixteenthNote;
                    break;
                case 2:
                    length = Core::TimeDivision::EighthNote;
                    break;
                case 3:
                    length = Core::TimeDivision::DottedEighthNote;
                    break;
                case 4:
                    length = Core::TimeDivision::QuarterNote;
                    break;
                case 5:
                    length = Core::TimeDivision::DottedQuarterNote;
                    break;
                case 6:
                    length = Core::TimeDivision::HalfNote;
                    break;
                case 7:
                    length = Core::TimeDivision::DottedHalfNote;
                    break;
                case 8:
                    length = Core::TimeDivision::WholeNote;
                    break;
                case 9:
                    length = Core::TimeDivision::DottedWholeNote;
                    break;
                default:
                    length = Core::TimeDivision::SixteenthNote;
                    break;
            }
            step->setNoteLength(length);
        }
    }
}

void StepComponent::setStep(Core::Step* newStep)
{
    step = newStep;

    if (step != nullptr)
    {
        noteValueSlider.setValue(step->getNote(), juce::dontSendNotification);
        velocitySlider.setValue(step->getVelocity(), juce::dontSendNotification);

        // Convert TimeDivision to slider value
        double lengthValue = 0.0;
        switch (step->getNoteLength())
        {
            case Core::TimeDivision::SixteenthNote:
                lengthValue = 0.0;
                break;
            case Core::TimeDivision::DottedSixteenthNote:
                lengthValue = 1.0;
                break;
            case Core::TimeDivision::EighthNote:
                lengthValue = 2.0;
                break;
            case Core::TimeDivision::DottedEighthNote:
                lengthValue = 3.0;
                break;
            case Core::TimeDivision::QuarterNote:
                lengthValue = 4.0;
                break;
            case Core::TimeDivision::DottedQuarterNote:
                lengthValue = 5.0;
                break;
            case Core::TimeDivision::HalfNote:
                lengthValue = 6.0;
                break;
            case Core::TimeDivision::DottedHalfNote:
                lengthValue = 7.0;
                break;
            case Core::TimeDivision::WholeNote:
                lengthValue = 8.0;
                break;
            case Core::TimeDivision::DottedWholeNote:
                lengthValue = 9.0;
                break;
            default:
                // For any other value, default to sixteenth note
                lengthValue = 0.0;
                break;
        }

        noteLengthSlider.setValue(lengthValue, juce::dontSendNotification);
    }
}

void StepComponent::setNoteValue(float value)
{
    noteValueSlider.setValue(value, juce::sendNotificationSync);
}

float StepComponent::getNoteValue() const
{
    return static_cast<float>(noteValueSlider.getValue());
}

void StepComponent::setVelocity(float value)
{
    velocitySlider.setValue(value, juce::sendNotificationSync);
}

float StepComponent::getVelocity() const
{
    return static_cast<float>(velocitySlider.getValue());
}

} // namespace Sirkus::UI
