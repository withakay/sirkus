#pragma once

#include "../Constants.h"
#include "../Identifiers.h"
#include "../JuceHeader.h"
#include "Step.h"
#include "Types.h"
#include "ValueTreeObject.h"
#include "TriggerBuffer.h"

#include <atomic>
#include <map>
#include <mutex>
#include <vector>

namespace Sirkus::Core {

class Pattern final : public ValueTreeObject
{
public:
    Pattern(ValueTree parentState, UndoManager& undoManagerToUse);

    struct props
    {
        static inline const TypedProperty<int> length{ID::Pattern::length, 16};
        static inline const TypedProperty<float> swingAmount{ID::Pattern::swingAmount, 0.0f};
        static inline const TypedProperty<TimeDivision> stepInterval{ID::Pattern::stepInterval, TimeDivision::SixteenthNote};
    };

    // Step manipulation
    void setStepEnabled(size_t stepIndex, bool enabled);
    void setStepNote(size_t stepIndex, uint8_t note) const;
    void setStepVelocity(size_t stepIndex, uint8_t velocity) const;
    void setStepProbability(size_t stepIndex, float probability) const;
    void setStepOffset(size_t stepIndex, float offset);
    void setStepSwingAffected(size_t stepIndex, bool affected);
    void setStepTrackId(size_t stepIndex, uint32_t trackId);
    void setStepNoteLength(size_t stepIndex, TimeDivision length);

    // Pattern parameters
    void setLength(size_t newLength)
    {
        setProperty(props::length, static_cast<int>(newLength));
    }

    void setSwingAmount(float amount)
    {
        setProperty(props::swingAmount, amount);
    }

    void setStepInterval(TimeDivision interval)
    {
        setProperty(props::stepInterval, interval);
    }

    size_t getLength() const
    {
        return static_cast<size_t>(getProperty(props::length));
    }

    float getSwingAmount() const
    {
        return getProperty(props::swingAmount);
    }

    TimeDivision getStepInterval() const
    {
        return getProperty(props::stepInterval);
    }

    // Step access
    Step& getStep(size_t stepIndex) const;
    bool isStepEnabled(size_t stepIndex) const;

    // Trigger map access
    const std::map<int, size_t>& getTriggerMap() const;

    // Get step timing information
    int getStepStartTick(size_t stepIndex) const;

    int getStepEndTick(size_t stepIndex) const
    {
        if (stepIndex >= MAX_STEPS)
            return 0;

        const int startTick = calculateStepTick(stepIndex);
        const auto& step = getStep(stepIndex);
        const int noteLengthTicks = step.getNoteLength(); // TimeDivision enum values are ticks

        // Handle pattern wrapping
        const int gridSpacing = stepIntervalToTicks(getProperty(props::stepInterval));
        const int gridLength = static_cast<int>(getProperty(props::length));
        const int patternLengthTicks = gridLength * gridSpacing;
        return (startTick + noteLengthTicks) % patternLengthTicks;
    }

private:
    std::array<TriggerBuffer, 2> triggerBuffers;
    std::atomic<size_t> activeBuffer{0};
    mutable std::mutex updateMutex;

    std::vector<std::unique_ptr<Step>> steps = std::vector<std::unique_ptr<Step>>(MAX_STEPS);

    void updateStepTiming(size_t stepIndex, bool acquireLock = false); // Set acquireLock=true if no lock is held
    void initializeStepTiming(size_t stepIndex);
    int calculateStepTick(size_t stepIndex) const;
    void ensureStepExists(size_t stepIndex);

    JUCE_LEAK_DETECTOR(Pattern)
};

} // namespace Sirkus::Core
