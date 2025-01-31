#pragma once

#include "TimingInfo.h"

namespace Sirkus::Core {
class InternalTransport
{
public:
    InternalTransport();

    void prepare(double sampleRate);
    void processBlock(int numSamples);

    // Get complete timing info
    [[nodiscard]] TimingInfo getTimingInfo() const;

    // Transport controls
    void setBpm(double newBpm);
    void setTimeSignature(int numerator, int denominator);
    void start();
    void stop();
    void setPositionInBars(int bar, int beat = 1, double tick = 0.0);

    [[nodiscard]] bool isPlaying() const { return playing; }

private:
    double sampleRate;
    double bpm;
    double ppqPosition;
    double samplesPerQuarterNote;
    bool playing;

    int timeSigNumerator;
    int timeSigDenominator;
    MusicalPosition musicalPosition;

    void updateTimingInfo();
    void updateMusicalPosition();
    [[nodiscard]] double ppqPositionToBeats(double ppq) const;
    [[nodiscard]] double beatsToNextBar(const MusicalPosition& pos) const;
};
} // namespace Sirkus::Core
