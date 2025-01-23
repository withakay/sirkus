#pragma once

#include <JuceHeader.h>
#include "Pattern.h"
#include "Scale.h"
#include "Types.h"
#include <memory>
#include <vector>
#include <atomic>

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
    
    // Scale settings
    void setScaleMode(ScaleMode mode) { scaleMode.store(mode, std::memory_order_release); }
    ScaleMode getScaleMode() const { return scaleMode.load(std::memory_order_acquire); }
    
    void setScale(Scale::Type type, uint8_t root = 0);
    void setCustomScale(const std::vector<uint8_t>& degrees, uint8_t root = 0);
    const Scale& getScale() const { return scale; }
    
    // Note quantization
    uint8_t quantizeNote(uint8_t note) const;
    
    // Get active steps for the current tick range
    std::vector<std::pair<uint64_t, const Step*>> getActiveSteps(uint64_t startTick, uint64_t numTicks) const;

private:
    const uint32_t trackId;
    std::unique_ptr<Pattern> currentPattern;
    
    std::atomic<uint8_t> midiChannel{1};
    std::atomic<ScaleMode> scaleMode{ScaleMode::Off};
    Scale scale;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};

} // namespace sirkus
