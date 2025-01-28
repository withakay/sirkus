#pragma once

#include <JuceHeader.h>
#include <deque>

namespace Sirkus::UI {

class MidiEventLog : public juce::Component, public juce::Timer
{
public:
    MidiEventLog();
    ~MidiEventLog() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    // Add a MIDI message to the log
    void logMidiMessage(const juce::MidiMessage& message);

private:
    struct LogEntry
    {
        juce::String description;
        juce::uint32 timestamp;

        LogEntry(const juce::String& desc, juce::uint32 time)
                : description(desc)
                , timestamp(time)
        {
        }
    };

    std::deque<LogEntry> eventLog;
    static constexpr size_t maxEvents = 100;
    static constexpr int refreshRate = 50; // ms

    juce::Font font{14.0f};
    float lineHeight = 16.0f;

    void trimLog();
    juce::String formatTimestamp(juce::uint32 timestamp) const;
    juce::String formatMidiMessage(const juce::MidiMessage& message) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiEventLog)
};

} // namespace Sirkus::UI
