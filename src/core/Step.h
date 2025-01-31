#pragma once

#include "../Identifiers.h"
#include "Types.h"
#include "ValueTreeObject.h"

#include "../JuceHeader.h"
#include <cstdint>

namespace Sirkus::Core {

class Step final : public ValueTreeObject
{
public:
    Step(ValueTree parentState, UndoManager& undoManagerToUse);

    // Copy constructor and assignment operator inherited from ValueTreeObject
    Step(const Step&) = default;
    Step& operator=(const Step&) = default;

    struct props
    {
        static inline const TypedProperty<bool> enabled{ID::Step::enabled, false};
        static inline const TypedProperty<uint8_t> note{ID::Step::note, 60};
        static inline const TypedProperty<uint8_t> velocity{ID::Step::velocity, 100};
        static inline const TypedProperty<float> probability{ID::Step::probability, 1.0f};
        static inline const TypedProperty<float> timingOffset{ID::Step::timingOffset, 0.0f};
        static inline const TypedProperty<bool> affectedBySwing{ID::Step::affectedBySwing, true};
        static inline const TypedProperty<int> triggerTick{ID::Step::triggerTick, 0};
        static inline const TypedProperty<uint32_t> trackId{ID::Step::trackId, 0};
        static inline const TypedProperty<NoteLength> noteLength{ID::Step::noteLength, NoteLength::Quarter};
    };

    // Property getters/setters
    bool isEnabled() const
    {
        return getProperty(props::enabled);
    }

    void setEnabled(bool value)
    {
        setProperty(props::enabled, value);
    }

    uint8_t getNote() const
    {
        return getProperty(props::note);
    }

    void setNote(uint8_t value)
    {
        setProperty(props::note, value);
    }

    uint8_t getVelocity() const
    {
        return getProperty(props::velocity);
    }

    void setVelocity(uint8_t value)
    {
        setProperty(props::velocity, value);
    }

    float getProbability() const
    {
        return getProperty(props::probability);
    }

    void setProbability(float value)
    {
        setProperty(props::probability, value);
    }

    float getTimingOffset() const
    {
        return getProperty(props::timingOffset);
    }

    void setTimingOffset(float value)
    {
        setProperty(props::timingOffset, value);
    }

    bool isAffectedBySwing() const
    {
        return getProperty(props::affectedBySwing);
    }

    void setAffectedBySwing(bool value)
    {
        setProperty(props::affectedBySwing, value);
    }

    int getTriggerTick() const
    {
        return getProperty(props::triggerTick);
    }

    void setTriggerTick(int value)
    {
        setProperty(props::triggerTick, value);
    }

    uint32_t getTrackId() const
    {
        return getProperty(props::trackId);
    }

    void setTrackId(uint32_t value)
    {
        setProperty(props::trackId, value);
    }

    NoteLength getNoteLength() const
    {
        return getProperty(props::noteLength);
    }

    void setNoteLength(NoteLength value)
    {
        setProperty(props::noteLength, value);
    }

    // Helper methods
    int getNoteLengthInTicks() const
    {
        return noteLengthToTicks(getNoteLength());
    }

private:
    JUCE_LEAK_DETECTOR(Step)
};

} // namespace Sirkus::Core
