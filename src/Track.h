#pragma once

#include "Pattern.h"
#include "Types.h"
#include <JuceHeader.h>
#include <atomic>
#include <memory>
#include <vector>

namespace sirkus {

class Track {
public:
    explicit Track(uint32_t id);

    // Pattern management
    void setPattern(std::unique_ptr<Pattern> newPattern);
    Pattern* getCurrentPattern() { return currentPattern.get(); }
    const Pattern* getCurrentPattern() const { return currentPattern.get(); }

    // Track properties
    uint32_t getId() const { return trackId; }
    uint8_t getMidiChannel() const { return midiChannel.load(std::memory_order_acquire); }
    void setMidiChannel(uint8_t channel) { midiChannel.store(channel, std::memory_order_release); }

    // Scale mode settings
    void setScaleMode(ScaleMode mode) { scaleMode.store(mode, std::memory_order_release); }
    ScaleMode getScaleMode() const { return scaleMode.load(std::memory_order_acquire); }

    // Get track information needed for step processing
    TrackInfo getTrackInfo() const {
      return TrackInfo{trackId, getMidiChannel(), getScaleMode()};
    }

    // Get active steps for the current tick range
    std::vector<std::pair<int, const Step*>> getActiveSteps(int startTick, int numTicks) const;

private:
    const uint32_t trackId;
    std::unique_ptr<Pattern> currentPattern;

    std::atomic<uint8_t> midiChannel{1};
    std::atomic<ScaleMode> scaleMode{ScaleMode::Off};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};

} // namespace sirkus
