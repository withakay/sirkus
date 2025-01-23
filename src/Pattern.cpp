#include "Pattern.h"
#include <cassert>

namespace sirkus {

void TriggerBuffer::addStep(uint64_t tick, uint32_t stepIndex) {
    // Remove any existing entry for this step
    auto existing = stepToTick.find(stepIndex);
    if (existing != stepToTick.end()) {
        tickToStep.erase(existing->second);
        stepToTick.erase(existing);
    }
    
    // Add new mappings
    tickToStep[tick] = stepIndex;
    stepToTick[stepIndex] = tick;
}

void TriggerBuffer::removeStep(uint32_t stepIndex) {
    auto it = stepToTick.find(stepIndex);
    if (it != stepToTick.end()) {
        tickToStep.erase(it->second);
        stepToTick.erase(it);
    }
}

bool TriggerBuffer::verifyIntegrity() const {
    if (tickToStep.size() != stepToTick.size()) return false;
    
    for (const auto& [tick, step] : tickToStep) {
        auto it = stepToTick.find(step);
        if (it == stepToTick.end() || it->second != tick) return false;
    }
    return true;
}

Pattern::Pattern() {
    // Initialize steps
    for (uint32_t i = 0; i < MAX_STEPS; ++i) {
        updateStepTiming(i);
    }
}

void Pattern::setStepEnabled(uint32_t stepIndex, bool enabled) {
    if (stepIndex >= MAX_STEPS) return;
    steps[stepIndex].enabled.store(enabled, std::memory_order_release);
    updateStepTiming(stepIndex);
}

void Pattern::setStepNote(uint32_t stepIndex, uint8_t note) {
    if (stepIndex >= MAX_STEPS) return;
    steps[stepIndex].note.store(note, std::memory_order_release);
}

void Pattern::setStepVelocity(uint32_t stepIndex, uint8_t velocity) {
    if (stepIndex >= MAX_STEPS) return;
    steps[stepIndex].velocity.store(velocity, std::memory_order_release);
}

void Pattern::setStepProbability(uint32_t stepIndex, float probability) {
    if (stepIndex >= MAX_STEPS) return;
    steps[stepIndex].probability.store(probability, std::memory_order_release);
}

void Pattern::setStepOffset(uint32_t stepIndex, float offset) {
    if (stepIndex >= MAX_STEPS) return;
    steps[stepIndex].timingOffset.store(offset, std::memory_order_release);
    updateStepTiming(stepIndex);
}

void Pattern::setStepSwingAffected(uint32_t stepIndex, bool affected) {
    if (stepIndex >= MAX_STEPS) return;
    steps[stepIndex].affectedBySwing.store(affected, std::memory_order_release);
    updateStepTiming(stepIndex);
}

void Pattern::setStepTrackId(uint32_t stepIndex, uint32_t trackId) {
    if (stepIndex >= MAX_STEPS) return;
    steps[stepIndex].trackId.store(trackId, std::memory_order_release);
}

void Pattern::setStepNoteLength(uint32_t stepIndex, NoteLength length) {
    if (stepIndex >= MAX_STEPS) return;
    steps[stepIndex].noteLength.store(length, std::memory_order_release);
}

void Pattern::setLength(uint32_t newLength) {
    if (newLength > MAX_STEPS) return;
    length.store(newLength, std::memory_order_release);
    
    // Update all steps as length affects tick calculation
    std::lock_guard<std::mutex> lock(updateMutex);
    for (uint32_t i = 0; i < newLength; ++i) {
        updateStepTiming(i);
    }
}

void Pattern::setSwingAmount(float amount) {
    swingAmount.store(amount, std::memory_order_release);
    
    // Update all swing-affected steps
    std::lock_guard<std::mutex> lock(updateMutex);
    for (uint32_t i = 0; i < length; ++i) {
        if (steps[i].affectedBySwing) {
            updateStepTiming(i);
        }
    }
}

void Pattern::setStepInterval(StepInterval interval) {
    stepInterval.store(interval, std::memory_order_release);
    
    // Update all steps as grid spacing affects tick calculation
    std::lock_guard<std::mutex> lock(updateMutex);
    for (uint32_t i = 0; i < length; ++i) {
        updateStepTiming(i);
    }
}

const Step& Pattern::getStep(uint32_t stepIndex) const {
    assert(stepIndex < MAX_STEPS);
    return steps[stepIndex];
}

uint64_t Pattern::calculateStepTick(uint32_t stepIndex) const {
    // Calculate base tick using step interval
    const uint64_t gridSpacing = stepIntervalToTicks(stepInterval.load(std::memory_order_acquire));
    uint64_t baseTick = stepIndex * gridSpacing;
    uint64_t finalTick = baseTick;
    
    // Apply swing if applicable
    if (steps[stepIndex].affectedBySwing && (stepIndex % 2) != 0) {
        finalTick += static_cast<uint64_t>(PPQN * swingAmount);
    }
    
    // Apply micro-timing offset
    float offset = steps[stepIndex].timingOffset;
    int64_t tickOffset = static_cast<int64_t>(PPQN * offset);
    
    // Handle wrapping for negative offsets
    const uint64_t patternLengthTicks = length * gridSpacing;
    finalTick = (finalTick + tickOffset + patternLengthTicks) % patternLengthTicks;
    
    return finalTick;
}

uint64_t Pattern::getStepStartTick(uint32_t stepIndex) const {
    if (stepIndex >= MAX_STEPS) return 0;
    return calculateStepTick(stepIndex);
}

uint64_t Pattern::getStepEndTick(uint32_t stepIndex) const {
    if (stepIndex >= MAX_STEPS) return 0;
    
    const uint64_t startTick = calculateStepTick(stepIndex);
    const uint64_t noteLengthTicks = steps[stepIndex].getNoteLengthInTicks();
    
    // Handle pattern wrapping
    const uint64_t gridSpacing = stepIntervalToTicks(stepInterval.load(std::memory_order_acquire));
    const uint64_t patternLengthTicks = length * gridSpacing;
    return (startTick + noteLengthTicks) % patternLengthTicks;
}

void Pattern::updateStepTiming(uint32_t stepIndex) {
    std::lock_guard<std::mutex> lock(updateMutex);
    
    size_t current = activeBuffer.load(std::memory_order_acquire);
    size_t inactive = 1 - current;
    
    // Work on inactive buffer
    auto& workingBuffer = triggerBuffers[inactive];
    // Copy maps from current buffer
    workingBuffer.tickToStep = triggerBuffers[current].tickToStep;
    workingBuffer.stepToTick = triggerBuffers[current].stepToTick;
    
    if (steps[stepIndex].enabled) {
        uint64_t finalTick = calculateStepTick(stepIndex);
        workingBuffer.addStep(finalTick, stepIndex);
    } else {
        workingBuffer.removeStep(stepIndex);
    }
    
    // Verify and swap
    assert(workingBuffer.verifyIntegrity());
    workingBuffer.dirty.store(true, std::memory_order_release);
    activeBuffer.store(inactive, std::memory_order_release);
}

const std::map<uint64_t, uint32_t>& Pattern::getTriggerMap() const {
    return triggerBuffers[activeBuffer.load(std::memory_order_acquire)].tickToStep;
}

} // namespace sirkus
