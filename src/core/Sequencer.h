#pragma once

#include "../Constants.h"
#include "../Identifiers.h"
#include "StepProcessor.h"
#include "TimingManager.h"
#include "Track.h"
#include "ValueTreeObject.h"

#include <JuceHeader.h>
#include <array>
#include <memory>
#include <vector>

namespace Sirkus::Core {

class Sequencer final : public ValueTreeWrapper
{
public:
    Sequencer(ValueTree parentState, UndoManager& undoManagerToUse);

    struct props
    {
        SIRKUS_DECLARE_TYPED_PROPERTY(float, swingAmount, 0.0f);
    };

    // Track Management
    Track* addTrack();                  // Returns nullptr if at MAX_TRACKS
    bool removeTrack(uint32_t trackId); // Can't remove last track
    Track* getTrack(uint32_t trackId);

    size_t getTrackCount() const;

    // Timing Control
    TimingManager& getTimingManager();

    // Audio Processing
    void prepare(double sampleRate);
    void processBlock(const juce::AudioPlayHead* playHead, int numSamples, juce::MidiBuffer& midiOut);

    // Global Parameters
    void setSwingAmount(const float amount);
    float getSwingAmount() const;

    void setScale(Scale::Type type, uint8_t root);
    void setCustomScale(const std::vector<uint8_t>& degrees, uint8_t root);

    Scale::Type getScaleType() const;
    uint8_t getScaleRoot() const;
    const std::vector<uint8_t>& getGlobalCustomDegrees() const;

private:
    uint32_t generateTrackId();
    void updateTrackSwing() const;

    TimingManager timingManager;
    StepProcessor stepProcessor;
    std::vector<std::unique_ptr<Track>> tracks;
    uint32_t nextTrackId{0};

    float swingAmount{0.0f};
    Scale globalScale{Scale::Type::Major}; // Current global scale
    Scale::Type scaleType{Scale::Type::Major};
    uint8_t scaleRoot{0};
    std::vector<uint8_t> globalCustomDegrees;
    double currentSampleRate{44100.0};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sequencer)
};

} // namespace Sirkus::Core
