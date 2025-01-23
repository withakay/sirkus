#pragma once

#include <JuceHeader.h>
#include <cmath>
#include <optional>
#include <utility>

namespace sirkus {

struct MusicalPosition
{
    int bar = 1;       // Starts at 1
    int beat = 1;      // Starts at 1
    double tick = 0.0; // Tick within the current beat

    bool operator==(const MusicalPosition& other) const
    {
        constexpr double epsilon = 1e-6; // Small value for floating point comparison
        return bar == other.bar && beat == other.beat && std::abs(tick - other.tick) < epsilon;
    }
} __attribute__((aligned(16)));

class InternalTransport; // Forward declaration

struct TimingInfo
{
    std::optional<double> ppqPosition;
    std::optional<double> bpm;
    std::optional<MusicalPosition> musicalPosition;
    std::optional<std::pair<int, int>> timeSignature;
    bool isPlaying = false;

    static TimingInfo fromPositionInfo(const juce::AudioPlayHead::PositionInfo& pos);
    static TimingInfo fromInternalTransport(const InternalTransport& transport);
} __attribute__((aligned(128)));

} // namespace sirkus
