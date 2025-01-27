#pragma once

#include "Constants.h"
#include "Step.h"
#include "Types.h"

#include <JuceHeader.h>
#include <array>
#include <atomic>
#include <map>
#include <mutex>

namespace Sirkus {

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

class Pattern
{
public:
    Pattern();

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
    void setLength(size_t newLength);
    void setSwingAmount(float amount);
    void setStepInterval(StepInterval interval);

    // Step access
    const Step& getStep(size_t stepIndex) const;

    size_t getLength() const
    {
        return length.load(std::memory_order_acquire);
    }

    float getSwingAmount() const
    {
        return swingAmount.load(std::memory_order_acquire);
    }

    StepInterval getStepInterval() const
    {
        return stepInterval.load(std::memory_order_acquire);
    }

    // Trigger map access
    const std::map<int, size_t>& getTriggerMap() const;

    // Get step timing information
    int getStepStartTick(size_t stepIndex) const;
    int getStepEndTick(size_t stepIndex) const;

private:
    std::array<Step, MAX_STEPS> steps;
    std::atomic<size_t> length{16};
    std::atomic<float> swingAmount{0.0f};
    std::atomic<StepInterval> stepInterval{StepInterval::Quarter}; // Grid spacing

    std::array<TriggerBuffer, 2> triggerBuffers;
    std::atomic<size_t> activeBuffer{0};
    mutable std::mutex updateMutex;

    // Internal helpers
    void updateStepTiming(size_t stepIndex, bool acquireLock = false); // Set acquireLock=true if no lock is held
    void initializeStepTiming(size_t stepIndex);                       // Lock-free initialization
    int calculateStepTick(size_t stepIndex) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Pattern)
};

} // namespace Sirkus
