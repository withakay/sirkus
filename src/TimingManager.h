#pragma once

#include <JuceHeader.h>
#include "TimingInfo.h"
#include "InternalTransport.h"

class TimingManager
{
public:
    TimingManager();

    void prepare(double sampleRate);
    void processBlock(juce::AudioPlayHead* playHead, int numSamples);

    // Timing info getters
    [[nodiscard]] std::optional<double> getPpqPosition() const;
    [[nodiscard]] std::optional<double> getBpm() const;
    [[nodiscard]] std::optional<MusicalPosition> getMusicalPosition() const;
    [[nodiscard]] std::optional<std::pair<int, int>> getTimeSignature() const;

    [[nodiscard]] bool isStandaloneMode() const { return standaloneMode; }

    // Host sync control
    void setHostSyncEnabled(bool enabled) { hostSyncEnabled = enabled; }
    [[nodiscard]] bool isHostSyncEnabled() const { return hostSyncEnabled; }

    // Transport control methods
    void setBpm(double newBpm) { internalTransport.setBpm(newBpm); }
    void setTimeSignature(int numerator, int denominator) { internalTransport.setTimeSignature(numerator, denominator); }
    void start() { internalTransport.start(); }
    void stop() { internalTransport.stop(); }
    [[nodiscard]] bool isPlaying() const { return internalTransport.isPlaying(); }
    void setPositionInBars(const int bar, const int beat = 1, const double tick = 0.0)
    {
        internalTransport.setPositionInBars(bar, beat, tick);
    }

private:
    InternalTransport internalTransport;
    bool standaloneMode;
    bool hostSyncEnabled = true;  // Default to syncing with host when available
    TimingInfo currentTiming;

    void updateHostPositionInfo(const juce::AudioPlayHead::PositionInfo& pos);
};