#include "Pattern.h"
#include <cassert>
#include <cstdint>

namespace sirkus {

void TriggerBuffer::addStep(int tick, size_t stepIndex)
{
    // Remove any existing entry for this step
    if (const auto existing = this->stepToTick.find(stepIndex); existing != this->stepToTick.end())
    {
        tickToStep.erase(existing->second);
        stepToTick.erase(existing);
    }

    // Add new mappings
    tickToStep[tick] = stepIndex;
    stepToTick[stepIndex] = tick;
}

void TriggerBuffer::removeStep(const size_t stepIndex)
{
    auto it = stepToTick.find(stepIndex);
    if (it != stepToTick.end())
    {
        tickToStep.erase(it->second);
        stepToTick.erase(it);
    }
}

bool TriggerBuffer::verifyIntegrity() const
{
    if (tickToStep.size() != stepToTick.size())
        return false;

    for (const auto& [tick, step] : tickToStep)
    {
        auto it = stepToTick.find(step);
        if (it == stepToTick.end() || it->second != tick)
            return false;
    }
    return true;
}

Pattern::Pattern()
{
    // Initialize steps
    for (size_t i = 0; i < MAX_STEPS; ++i)
    {
        updateStepTiming(i);
    }
}

void Pattern::setStepEnabled(const size_t stepIndex, bool enabled)
{
    if (stepIndex >= MAX_STEPS)
        return;
    this->steps[stepIndex].enabled.store(enabled, std::memory_order_release);
    updateStepTiming(stepIndex);
}

void Pattern::setStepNote(const size_t stepIndex, uint8_t note)
{
    if (stepIndex >= MAX_STEPS)
        return;
    this->steps[stepIndex].note.store(note, std::memory_order_release);
}

void Pattern::setStepVelocity(const size_t stepIndex, uint8_t velocity)
{
    if (stepIndex >= MAX_STEPS)
        return;
    this->steps[stepIndex].velocity.store(velocity, std::memory_order_release);
}

void Pattern::setStepProbability(const size_t stepIndex, float probability)
{
    if (stepIndex >= MAX_STEPS)
        return;
    this->steps[stepIndex].probability.store(probability, std::memory_order_release);
}

void Pattern::setStepOffset(const size_t stepIndex, float offset)
{
    if (stepIndex >= MAX_STEPS)
        return;
    this->steps[stepIndex].timingOffset.store(offset, std::memory_order_release);
    updateStepTiming(stepIndex);
}

void Pattern::setStepSwingAffected(const size_t stepIndex, bool affected)
{
    if (stepIndex >= MAX_STEPS)
        return;
    this->steps[stepIndex].affectedBySwing.store(affected, std::memory_order_release);
    updateStepTiming(stepIndex);
}

void Pattern::setStepTrackId(const size_t stepIndex, uint32_t trackId)
{
    if (stepIndex >= MAX_STEPS)
        return;
    this->steps[stepIndex].trackId.store(trackId, std::memory_order_release);
}

void Pattern::setStepNoteLength(const size_t stepIndex, NoteLength noteLength)
{
    if (stepIndex >= MAX_STEPS)
        return;
    this->steps[stepIndex].noteLength.store(noteLength, std::memory_order_release);
}

void Pattern::setLength(size_t newLength)
{
    if (newLength > MAX_STEPS)
        return;

    this->length.store(newLength, std::memory_order_release);

    // Update all steps as length affects tick calculation
    std::lock_guard<std::mutex> lock(updateMutex);
    for (size_t i = 0; i < newLength; ++i)
    {
        updateStepTiming(i);
    }
}

void Pattern::setSwingAmount(float amount)
{
    this->swingAmount.store(amount, std::memory_order_release);

    // Update all swing-affected steps
    std::lock_guard<std::mutex> const lock(updateMutex);
    const size_t currentLength = this->length.load(std::memory_order_acquire);
    for (size_t i = 0; i < currentLength; ++i)
    {
        if (this->steps[i].affectedBySwing.load(std::memory_order_acquire))
        {
            updateStepTiming(i);
        }
    }
}

void Pattern::setStepInterval(StepInterval interval)
{
    this->stepInterval.store(interval, std::memory_order_release);

    // Update all steps as grid spacing affects tick calculation
    std::lock_guard<std::mutex> lock(updateMutex);
    const size_t currentLength = this->length.load(std::memory_order_acquire);
    for (size_t i = 0; i < currentLength; ++i)
    {
        updateStepTiming(i);
    }
}

const Step& Pattern::getStep(const size_t stepIndex) const
{
    assert(stepIndex < MAX_STEPS);
    return this->steps[stepIndex];
}

int Pattern::calculateStepTick(const size_t stepIndex) const
{
    // Calculate base tick using step interval
    const int gridSpacing = stepIntervalToTicks(this->stepInterval.load(std::memory_order_acquire));
    const int baseTick = static_cast<int>(stepIndex) * gridSpacing;
    int finalTick = baseTick;

    // Apply swing if applicable
    if (this->steps[stepIndex].affectedBySwing.load(std::memory_order_acquire) && (stepIndex % 2) != 0)
    {
        finalTick += static_cast<int>(PPQN * this->swingAmount.load(std::memory_order_acquire));
    }

    // Apply micro-timing offset
    const float offset = this->steps[stepIndex].timingOffset.load(std::memory_order_acquire);
    const int tickOffset = static_cast<int>(PPQN * offset);

    // Handle wrapping for negative offsets
    const int gridLength = static_cast<int>(this->length.load(std::memory_order_acquire));
    const int patternLengthTicks = gridLength * gridSpacing;
    return (finalTick + tickOffset + patternLengthTicks) % patternLengthTicks;
}

int Pattern::getStepStartTick(const size_t stepIndex) const
{
    if (stepIndex >= MAX_STEPS)
        return 0;
    return calculateStepTick(stepIndex);
}

int Pattern::getStepEndTick(const size_t stepIndex) const
{
    if (stepIndex >= MAX_STEPS)
        return 0;

    const int startTick = calculateStepTick(stepIndex);
    const int noteLengthTicks = steps[stepIndex].getNoteLengthInTicks();

    // Handle pattern wrapping
    const int gridSpacing = stepIntervalToTicks(this->stepInterval.load(std::memory_order_acquire));
    const int gridLength = static_cast<int>(this->length.load(std::memory_order_acquire));
    const int patternLengthTicks = gridLength * gridSpacing;
    return (startTick + noteLengthTicks) % patternLengthTicks;
}

void Pattern::updateStepTiming(const size_t stepIndex)
{
    std::lock_guard<std::mutex> lock(updateMutex);

    size_t current = this->activeBuffer.load(std::memory_order_acquire);
    size_t inactive = 1 - current;

    // Work on inactive buffer
    auto& workingBuffer = triggerBuffers[inactive];
    // Copy maps from current buffer
    workingBuffer.tickToStep = triggerBuffers[current].tickToStep;
    workingBuffer.stepToTick = triggerBuffers[current].stepToTick;

    if (steps[stepIndex].enabled.load(std::memory_order_acquire))
    {
        int finalTick = calculateStepTick(stepIndex);
        workingBuffer.addStep(finalTick, stepIndex);
    }
    else
    {
        workingBuffer.removeStep(stepIndex);
    }

    // Verify and swap
    assert(workingBuffer.verifyIntegrity());
    workingBuffer.dirty.store(true, std::memory_order_release);
    this->activeBuffer.store(inactive, std::memory_order_release);
}

const std::map<int, size_t>& Pattern::getTriggerMap() const
{
    return triggerBuffers[this->activeBuffer.load(std::memory_order_acquire)].tickToStep;
}

} // namespace sirkus
