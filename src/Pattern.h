#pragma once

#include "Constants.h"
#include "Step.h"
#include <JuceHeader.h>
#include <array>
#include <atomic>
#include <map>
#include <mutex>

namespace sirkus {

// Forward declaration
class Pattern;

struct TriggerBuffer {
    std::map<uint64_t, uint32_t> tickToStep;
    std::map<uint32_t, uint64_t> stepToTick;
    std::atomic<bool> dirty{false};
    
    void addStep(uint64_t tick, uint32_t stepIndex);
    void removeStep(uint32_t stepIndex);
    bool verifyIntegrity() const;
    
    // Allow copying of maps but reset dirty flag
    TriggerBuffer& operator=(const TriggerBuffer& other) {
        tickToStep = other.tickToStep;
        stepToTick = other.stepToTick;
        dirty.store(false);
        return *this;
    }
};

class Pattern {
public:
    Pattern();
    
    // Step manipulation
    void setStepEnabled(uint32_t stepIndex, bool enabled);
    void setStepNote(uint32_t stepIndex, uint8_t note);
    void setStepVelocity(uint32_t stepIndex, uint8_t velocity);
    void setStepProbability(uint32_t stepIndex, float probability);
    void setStepOffset(uint32_t stepIndex, float offset);
    void setStepSwingAffected(uint32_t stepIndex, bool affected);
    void setStepTrackId(uint32_t stepIndex, uint32_t trackId);
    void setStepNoteLength(uint32_t stepIndex, NoteLength length);
    
    // Pattern parameters
    void setLength(uint32_t newLength);
    void setSwingAmount(float amount);
    void setStepInterval(StepInterval interval);
    
    // Step access
    const Step& getStep(uint32_t stepIndex) const;
    uint32_t getLength() const { return length.load(std::memory_order_acquire); }
    float getSwingAmount() const { return swingAmount.load(std::memory_order_acquire); }
    StepInterval getStepInterval() const { return stepInterval.load(std::memory_order_acquire); }
    
    // Trigger map access
    const std::map<uint64_t, uint32_t>& getTriggerMap() const;
    
    // Get step timing information
    uint64_t getStepStartTick(uint32_t stepIndex) const;
    uint64_t getStepEndTick(uint32_t stepIndex) const;

private:
    std::array<Step, MAX_STEPS> steps;
    std::atomic<uint32_t> length{16};
    std::atomic<float> swingAmount{0.0f};
    std::atomic<StepInterval> stepInterval{StepInterval::Quarter};  // Grid spacing
    
    std::array<TriggerBuffer, 2> triggerBuffers;
    std::atomic<size_t> activeBuffer{0};
    mutable std::mutex updateMutex;
    
    // Internal helpers
    void updateStepTiming(uint32_t stepIndex);
    uint64_t calculateStepTick(uint32_t stepIndex) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Pattern)
};

} // namespace sirkus
