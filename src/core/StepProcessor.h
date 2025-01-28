#pragma once

#include "Scale.h"
#include "Types.h"
#include <JuceHeader.h>
#include <memory>
#include <vector>

namespace Sirkus::Core {

// Forward declarations
class Track;
struct Step;

class StepProcessor
{
public:
    StepProcessor();
    ~StepProcessor();

    // Process steps and generate MIDI output
    void processSteps(
        const std::vector<std::pair<int, const Step*>>& steps,
        const TrackInfo& trackInfo,
        const Scale& scale,
        int startTick,
        int numTicks,
        juce::MidiBuffer& midiOut);

private:
    // Helper methods
    static void processStep(
        const Step& step,
        const TrackInfo& trackInfo,
        const Scale& scale,
        int triggerTick,
        int startTick,
        int numTicks,
        juce::MidiBuffer& midiOut);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepProcessor)
};

} // namespace Sirkus::Core
