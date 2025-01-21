#include "InternalTransport.h"
#include "MidiEventGenerator.h"
#include <utility>

InternalTransport::InternalTransport()
    : sampleRate(44100.0)
    , bpm(120.0)
    , ppqPosition(0.0)
    , samplesPerQuarterNote(0.0)
    , playing(false)
    , timeSigNumerator(4)
    , timeSigDenominator(4)
{
    updateTimingInfo();
}

void InternalTransport::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    updateTimingInfo();
}

void InternalTransport::processBlock(int numSamples)
{
    if (!playing)
        return;

    ppqPosition += (numSamples / samplesPerQuarterNote);
    updateMusicalPosition();
}

TimingInfo InternalTransport::getTimingInfo() const
{
    TimingInfo info;
    info.ppqPosition = ppqPosition;
    info.bpm = bpm;
    info.musicalPosition = musicalPosition;
    info.timeSignature = std::make_pair(timeSigNumerator, timeSigDenominator);
    info.isPlaying = playing;
    return info;
}

void InternalTransport::setBpm(double newBpm)
{
    bpm = newBpm;
    updateTimingInfo();
}

void InternalTransport::setTimeSignature(int numerator, int denominator)
{
    timeSigNumerator = numerator;
    timeSigDenominator = denominator;
    updateMusicalPosition();
}

void InternalTransport::start()
{
    playing = true;
}

void InternalTransport::stop()
{
    playing = false;
}

void InternalTransport::setPositionInBars(int bar, int beat, double tick)
{
    musicalPosition.bar = bar;
    musicalPosition.beat = beat;
    musicalPosition.tick = tick;

    // Convert bar/beat/tick to PPQ
    const double beatsPerBar = timeSigNumerator * (4.0 / timeSigDenominator);
    const double totalBeats = (bar - 1) * beatsPerBar + (beat - 1) + (tick / MidiEventGenerator::PPQN);
    ppqPosition = totalBeats * (4.0 / timeSigDenominator);
}

void InternalTransport::updateTimingInfo()
{
    samplesPerQuarterNote = (60.0 / bpm) * sampleRate;
}

void InternalTransport::updateMusicalPosition()
{
    const auto timeSig = std::make_pair(timeSigNumerator, timeSigDenominator);
    const double beatsPerBar = timeSig.first * (4.0 / timeSig.second);
    const double totalBeats = ppqPosition * (timeSig.second / 4.0);

    musicalPosition.bar = static_cast<int>(totalBeats / beatsPerBar) + 1;
    musicalPosition.beat = static_cast<int>(std::fmod(totalBeats, beatsPerBar)) + 1;
    musicalPosition.tick = std::fmod(totalBeats, 1.0) * MidiEventGenerator::PPQN;
}

double InternalTransport::ppqPositionToBeats(double ppq) const
{
    return ppq * (timeSigDenominator / 4.0);
}

double InternalTransport::beatsToNextBar(const MusicalPosition& pos) const
{
    const double beatsPerBar = timeSigNumerator * (4.0 / timeSigDenominator);
    return beatsPerBar - ((pos.beat - 1) + (pos.tick / MidiEventGenerator::PPQN));
}