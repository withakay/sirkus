#pragma once

#include "Constants.h"
#include "TimingManager.h"
#include "Track.h"
#include "StepProcessor.h"
#include <JuceHeader.h>
#include <array>
#include <memory>
#include <vector>

namespace sirkus {

class Sequencer {
public:
    Sequencer();

    // Track Management
    Track *addTrack();                  // Returns nullptr if at MAX_TRACKS
    bool removeTrack(uint32_t trackId); // Can't remove last track
    Track* getTrack(uint32_t trackId);
    const Track* getTrack(uint32_t trackId) const;
    size_t getTrackCount() const { return tracks.size(); }

    // Timing Control
    TimingManager& getTimingManager() { return timingManager; }
    const TimingManager& getTimingManager() const { return timingManager; }

    // Audio Processing
    void prepare(double sampleRate);
    void processBlock(juce::AudioPlayHead* playHead, int numSamples, juce::MidiBuffer& midiOut);

    // Global Parameters
    void setGlobalSwing(float amount);
    float getGlobalSwing() const { return globalSwing; }

    void setGlobalScale(Scale::Type type, uint8_t root);
    void setGlobalCustomScale(const std::vector<uint8_t>& degrees, uint8_t root);
    Scale::Type getGlobalScaleType() const { return globalScaleType; }
    uint8_t getGlobalScaleRoot() const { return globalScaleRoot; }
    const std::vector<uint8_t>& getGlobalCustomDegrees() const { return globalCustomDegrees; }

private:
    uint32_t generateTrackId();
    void updateTrackSwing() const;
    void updateTrackScale();

    TimingManager timingManager;
    StepProcessor stepProcessor;
    std::vector<std::unique_ptr<Track>> tracks;
    uint32_t nextTrackId{0};

    float globalSwing{0.0f};
    Scale globalScale{Scale::Type::Major}; // Current global scale
    Scale::Type globalScaleType{Scale::Type::Major};
    uint8_t globalScaleRoot{0};
    std::vector<uint8_t> globalCustomDegrees;
    double currentSampleRate{44100.0};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sequencer)
};

} // namespace sirkus
