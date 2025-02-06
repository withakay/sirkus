#pragma once

#include "../Identifiers.h"
#include "Pattern.h"
#include "Types.h"
#include "ValueTreeObject.h"

#include "../JuceHeader.h"
#include <memory>
#include <vector>

namespace Sirkus::Core {

class Track final : public ValueTreeObject
{
public:
    Track(ValueTree parentState, UndoManager& undoManagerToUse, uint32_t id);

    struct props
    {
        static inline const TypedProperty<uint32_t> trackId{ID::Track::trackId, 0};
        static inline const TypedProperty<uint8_t> midiChannel{ID::Track::midiChannel, 1};
        static inline const TypedProperty<ScaleMode> scaleMode{ID::Track::scaleMode, ScaleMode::Off};
    };

    // Pattern management
    Pattern& getCurrentPattern() const;

    // Track properties
    uint32_t getId() const
    {
        return getProperty(props::trackId);
    }

    uint8_t getMidiChannel() const
    {
        return getProperty(props::midiChannel);
    }

    void setMidiChannel(uint8_t channel)
    {
        setProperty(props::midiChannel, channel);
    }

    // Scale mode settings
    void setScaleMode(ScaleMode mode)
    {
        setProperty(props::scaleMode, mode);
    }

    ScaleMode getScaleMode() const
    {
        return getProperty(props::scaleMode);
    }

    // Get track information needed for step processing
    TrackInfo getTrackInfo() const
    {
        return TrackInfo{getId(), getMidiChannel(), getScaleMode()};
    }

    // Get active steps for the current tick range
    std::vector<std::pair<int, const Step*>> getActiveSteps(int startTick, int numTicks) const;

private:
    void ensurePatternExists();
    std::unique_ptr<Pattern> currentPattern;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};

} // namespace Sirkus::Core
