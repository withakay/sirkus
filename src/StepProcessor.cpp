#include "StepProcessor.h"
#include "Track.h"
#include "Constants.h"
#include <algorithm>

namespace Sirkus {

StepProcessor::StepProcessor() = default;

StepProcessor::~StepProcessor() = default;

void StepProcessor::processSteps(
    const std::vector<std::pair<int, const Step*>>& steps,
    const TrackInfo& trackInfo,
    const Scale& scale,
    int startTick,
    int numTicks,
    juce::MidiBuffer& midiOut)
{
    // Process each active step
    for (const auto& [triggerTick, step] : steps)
    {
        if (step &&
            juce::Random::getSystemRandom().nextFloat() <= step->probability)
        {
            processStep(
                *step,
                trackInfo,
                scale,
                triggerTick,
                startTick,
                numTicks,
                midiOut);
        }
    }
}

void StepProcessor::processStep(
    const Step& step,
    const TrackInfo& trackInfo,
    const Scale& scale,
    const int triggerTick,
    const int startTick,
    const int numTicks,
    juce::MidiBuffer& midiOut)
{
    const int noteOnOffset = static_cast<int>((triggerTick - startTick));

    // Apply scale quantization based on mode
    uint8_t finalNote = step.note;
    switch (trackInfo.scaleMode)
    {
        case ScaleMode::Off:
            break;
        case ScaleMode::QuantizeUp:
            finalNote = scale.quantizeUp(step.note);
            break;
        case ScaleMode::QuantizeDown:
            finalNote = scale.quantizeDown(step.note);
            break;
        case ScaleMode::QuantizeRandom:
            finalNote = scale.quantizeRandom(step.note);
            break;
        default:
            break;
    }

    const uint8_t channel = trackInfo.midiChannel;
    const uint8_t velocity = step.velocity;

    // Calculate note-off position based on note length
    const auto noteLengthTicks = step.getNoteLengthInTicks();
    const auto noteOffTick = triggerTick + noteLengthTicks;
    const int noteOffOffset = noteOffTick - startTick;

    // Add note-on event if it falls within this block
    if (noteOnOffset >= 0 && noteOnOffset < numTicks)
    {
        midiOut.addEvent(
            juce::MidiMessage::noteOn(channel, finalNote, velocity),
            noteOnOffset);
    }

    // Add note-off event if it falls within this block
    if (noteOffOffset >= 0 && noteOffOffset < numTicks)
    {
        midiOut.addEvent(
            juce::MidiMessage::noteOff(channel, finalNote),
            noteOffOffset);
    }
}

} // namespace Sirkus
