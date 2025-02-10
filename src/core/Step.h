#pragma once

#include "../Identifiers.h"
#include "../JuceHeader.h"
#include "Types.h"
#include "ValueTreeObject.h"
#include "juce_data_structures/juce_data_structures.h"

#include <cstdint>

namespace Sirkus::Core {

class Step final : public ValueTreeObject
{
public:
    // Constructor for creating a new step that creates new ValueTree state
    Step(ValueTree parentState, UndoManager& undoManagerToUse, int index);

    // Constructor for creating a step from an existing ValueTree state
    Step(ValueTree existingState, UndoManager& undoManagerToUse, bool useExistingState);

    ~Step() override = default;

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
        static inline const TypedProperty<TimeDivision> noteLength{ID::Step::noteLength, TimeDivision::SixteenthNote};
    };

    // Property getters/setters
    bool isEnabled() const
    {
        return getProperty(props::enabled);
    }

    void setEnabled(const bool value)
    {
        setProperty(props::enabled, value);
    }

    uint8_t getNote() const
    {
        return getProperty(props::note);
    }

    void setNote(const uint8_t value)
    {
        setProperty(props::note, value);
    }

    uint8_t getVelocity() const
    {
        return getProperty(props::velocity);
    }

    void setVelocity(const uint8_t value)
    {
        setProperty(props::velocity, value);
    }

    float getProbability() const
    {
        return getProperty(props::probability);
    }

    void setProbability(const float value)
    {
        setProperty(props::probability, value);
    }

    float getTimingOffset() const
    {
        return getProperty(props::timingOffset);
    }

    void setTimingOffset(const float value)
    {
        setProperty(props::timingOffset, value);
    }

    bool isAffectedBySwing() const
    {
        return getProperty(props::affectedBySwing);
    }

    void setAffectedBySwing(const bool value)
    {
        setProperty(props::affectedBySwing, value);
    }

    int getTriggerTick() const
    {
        return getProperty(props::triggerTick);
    }

    void setTriggerTick(const int value)
    {
        setProperty(props::triggerTick, value);
    }

    uint32_t getTrackId() const
    {
        return getProperty(props::trackId);
    }

    void setTrackId(const uint32_t value)
    {
        setProperty(props::trackId, value);
    }

    TimeDivision getNoteLength() const
    {
        return getProperty(props::noteLength);
    }

    void setNoteLength(const TimeDivision value)
    {
        setProperty(props::noteLength, value);
    }

    // Helper methods
    int getNoteLengthInTicks() const
    {
        return getNoteLength();
    }

private:
    JUCE_LEAK_DETECTOR(Step)
};

} // namespace Sirkus::Core
