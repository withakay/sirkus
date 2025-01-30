#include "MidiEventLog.h"

namespace Sirkus::UI {

MidiEventLog::MidiEventLog()
{
    startTimer(refreshRate);
    setOpaque(true);
}

MidiEventLog::~MidiEventLog()
{
    stopTimer();
}

void MidiEventLog::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setFont(font);
    g.setColour(juce::Colours::lightgreen);

    int y = 4;
    for (const auto& entry : eventLog)
    {
        constexpr int x = 4;
        g.drawText(
            formatTimestamp(entry.timestamp) + " " + entry.description,
            x,
            y,
            getWidth() - 2 * x,
            lineHeight,
            juce::Justification::left,
            true);
        y += lineHeight;
    }
}

void MidiEventLog::resized()
{
    // Component size managed by parent
}

void MidiEventLog::timerCallback()
{
    repaint();
}

void MidiEventLog::logMidiMessage(const juce::MidiMessage& message)
{
    // Use MessageManager to safely post from audio thread
    juce::MessageManager::callAsync(
        [this, message]() {
            eventLog.emplace_front(formatMidiMessage(message), juce::Time::getMillisecondCounter());
            trimLog();
        });
}

void MidiEventLog::trimLog()
{
    while (eventLog.size() > maxEvents)
        eventLog.pop_back();
}

juce::String MidiEventLog::formatTimestamp(juce::uint32 timestamp) const
{
    auto currentTime = juce::Time::getMillisecondCounter();
    auto delta = currentTime - timestamp;
    return juce::String(delta) + "ms";
}

juce::String MidiEventLog::formatMidiMessage(const juce::MidiMessage& message) const
{
    if (message.isNoteOn())
        return "Note On: " + juce::MidiMessage::getMidiNoteName(message.getNoteNumber(), true, true, 4) +
               " vel:" + juce::String(message.getVelocity()) + " ch:" + juce::String(message.getChannel());

    if (message.isNoteOff())
        return "Note Off: " + juce::MidiMessage::getMidiNoteName(message.getNoteNumber(), true, true, 4) +
               " ch:" + juce::String(message.getChannel());

    if (message.isController())
        return "CC: " + juce::String(message.getControllerNumber()) +
               " val:" + juce::String(message.getControllerValue()) + " ch:" + juce::String(message.getChannel());

    if (message.isProgramChange())
        return "Program: " + juce::String(message.getProgramChangeNumber()) +
               " ch:" + juce::String(message.getChannel());

    if (message.isPitchWheel())
        return "Pitch Wheel: " + juce::String(message.getPitchWheelValue()) +
               " ch:" + juce::String(message.getChannel());

    return message.getDescription();
}

} // namespace Sirkus::UI
