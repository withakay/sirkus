#pragma once

#include "../Constants.h"
#include "../Identifiers.h"
#include "Step.h"
#include "Types.h"
#include "ValueTreeObject.h"

#include "../JuceHeader.h"
#include <atomic>
#include <map>
#include <mutex>
#include <vector>

namespace Sirkus::Core {

struct TriggerBuffer
{
    std::map<int, size_t> tickToStep; // Maps tick positions to step indices
    std::map<size_t, int> stepToTick; // Maps step indices to tick positions
    std::atomic<bool> dirty{false};

    void addStep(int tick, size_t stepIndex);
    void removeStep(size_t stepIndex);
    bool verifyIntegrity() const;

    // Allow copying of maps but reset dirty flag
    TriggerBuffer& operator=(const TriggerBuffer& other)
    {
        tickToStep = other.tickToStep;
        stepToTick = other.stepToTick;
        dirty.store(false);
        return *this;
    }
};

class Pattern final : public ValueTreeObject
{
public:
    Pattern(ValueTree parentState, UndoManager& undoManagerToUse);

    // Copy constructor and assignment operator inherited from ValueTreeObject
    Pattern(const Pattern&) = delete;
    Pattern& operator=(const Pattern&) = delete;

    struct props
    {
        static inline const TypedProperty<int> length{ID::Pattern::length, 16};
        static inline const TypedProperty<float> swingAmount{ID::Pattern::swingAmount, 0.0f};
        static inline const TypedProperty<StepInterval> stepInterval{ID::Pattern::stepInterval, StepInterval::Quarter};
    };

    // Step manipulation
    void setStepEnabled(size_t stepIndex, bool enabled);
    void setStepNote(size_t stepIndex, uint8_t note);
    void setStepVelocity(size_t stepIndex, uint8_t velocity);
    void setStepProbability(size_t stepIndex, float probability);
    void setStepOffset(size_t stepIndex, float offset);
    void setStepSwingAffected(size_t stepIndex, bool affected);
    void setStepTrackId(size_t stepIndex, uint32_t trackId);
    void setStepNoteLength(size_t stepIndex, NoteLength length);

    // Pattern parameters
    void setLength(size_t newLength)
    {
        setProperty(props::length, static_cast<int>(newLength));
    }

    void setSwingAmount(float amount)
    {
        setProperty(props::swingAmount, amount);
    }

    void setStepInterval(StepInterval interval)
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

    StepInterval getStepInterval() const
    {
        return getProperty(props::stepInterval);
    }

    // Step access
    Step getStepObject(size_t stepIndex) const;
    bool isStepEnabled(size_t stepIndex) const;

    // Trigger map access
    const std::map<int, size_t>& getTriggerMap() const;

    // Get step timing information
    int getStepStartTick(size_t stepIndex) const;
    int getStepEndTick(size_t stepIndex) const;

private:
    std::array<TriggerBuffer, 2> triggerBuffers;
    std::atomic<size_t> activeBuffer{0};
    mutable std::mutex updateMutex;

    // Internal helpers
    void updateStepTiming(size_t stepIndex, bool acquireLock = false); // Set acquireLock=true if no lock is held
    void initializeStepTiming(size_t stepIndex);                       // Lock-free initialization
    int calculateStepTick(size_t stepIndex) const;
    void ensureStepExists(size_t stepIndex);

    JUCE_LEAK_DETECTOR(Pattern)
};

} // namespace Sirkus::Core
