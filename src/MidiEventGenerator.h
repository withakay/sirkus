//
// Created by Jack Rutherford on 20/01/2025.
//

#pragma once

#include <JuceHeader.h>

class MidiEventGenerator
{
public:
    static constexpr int PPQN = 960;

    MidiEventGenerator();
    ~MidiEventGenerator() = default;

    void prepare(double sampleRate);
    void generateEvents(double ppqPosition, double bpm, int numSamples, juce::MidiBuffer& midiMessages) const;

private:
    static void addNoteEvents(juce::MidiBuffer& midiMessages, int samplePos, int numSamples);

    double currentSampleRate;
};