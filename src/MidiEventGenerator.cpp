//
// Created by Jack Rutherford on 20/01/2025.
//
#include "MidiEventGenerator.h"

MidiEventGenerator::MidiEventGenerator()
    : currentSampleRate(44100.0)
{
}

void MidiEventGenerator::prepare(double sampleRate)
{
    currentSampleRate = sampleRate;
}

void MidiEventGenerator::generateEvents(double ppqPosition, double bpm, int numSamples, juce::MidiBuffer& midiMessages) const
{
    if (ppqPosition < 0 || bpm <= 0)
        return;

    // Convert PPQ position to ticks
    const double currentTick = ppqPosition * PPQN;
    const double samplesPerTick = (60.0 / bpm / PPQN) * currentSampleRate;
    const double samplesToTicks = 1.0 / samplesPerTick;

    // Calculate start and end ticks for this block
    const double startTick = currentTick;
    const double endTick = startTick + (numSamples * samplesToTicks);

    // Find the next beat tick (multiple of PPQN)
    double nextBeatTick = ceil(startTick / PPQN) * PPQN;

    // Add MIDI events for all beats that occur in this block
    while (nextBeatTick < endTick)
    {
        if (const int samplePos = static_cast<int> ((nextBeatTick - startTick) * samplesPerTick); samplePos >= 0 && samplePos < numSamples)
        {
            addNoteEvents(midiMessages, samplePos, numSamples);
        }

        nextBeatTick += PPQN;  // Move to next beat
    }
}

void MidiEventGenerator::addNoteEvents(juce::MidiBuffer& midiMessages, const int samplePos, const int numSamples)
{
    static constexpr int CHANNEL = 1;
    static constexpr int NOTE_NUMBER = 60;  // Middle C
    static constexpr uint8 VELOCITY = 100;
    static constexpr int NOTE_DURATION_SAMPLES = 100;

    midiMessages.addEvent(juce::MidiMessage::noteOn(CHANNEL, NOTE_NUMBER, VELOCITY), samplePos);

    if (const int noteOffSample = samplePos + NOTE_DURATION_SAMPLES; noteOffSample < numSamples)
    {
        midiMessages.addEvent(juce::MidiMessage::noteOff(CHANNEL, NOTE_NUMBER), noteOffSample);
    }
}