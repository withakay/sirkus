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

    if (!std::ranges::all_of(tickToStep, [this](const auto &pair) {
          const auto &[tick, step] = pair;
          auto it = stepToTick.find(step);
          return it != stepToTick.end() && it->second == tick;
        })) {
      return false;
    }
    return true;
}

Pattern::Pattern()
{
  // Initialize steps without locking
  for (size_t i = 0; i < MAX_STEPS; ++i) {
    initializeStepTiming(i);
  }
}

void Pattern::initializeStepTiming(const size_t stepIndex) {
  // During construction, we only need to initialize buffer 0
  if (steps[stepIndex].enabled) {
    int finalTick = calculateStepTick(stepIndex);
    triggerBuffers[0].addStep(finalTick, stepIndex);
  }

  // Mark as initialized
  triggerBuffers[0].dirty.store(true, std::memory_order_release);
  // Set buffer 0 as active
  this->activeBuffer.store(0, std::memory_order_release);
}

void Pattern::setStepEnabled(const size_t stepIndex, bool enabled)
{
    if (stepIndex >= MAX_STEPS)
        return;
    this->steps[stepIndex].enabled = enabled;
    updateStepTiming(stepIndex, true); // Acquire lock for single step update
}

void Pattern::setStepNote(const size_t stepIndex, uint8_t note)
{
    if (stepIndex >= MAX_STEPS)
        return;
    this->steps[stepIndex].note = note;
}

void Pattern::setStepVelocity(const size_t stepIndex, uint8_t velocity)
{
    if (stepIndex >= MAX_STEPS)
        return;
    this->steps[stepIndex].velocity = velocity;
}

void Pattern::setStepProbability(const size_t stepIndex, float probability)
{
    if (stepIndex >= MAX_STEPS)
        return;
    this->steps[stepIndex].probability = probability;
}

void Pattern::setStepOffset(const size_t stepIndex, float offset)
{
    if (stepIndex >= MAX_STEPS)
        return;
    this->steps[stepIndex].timingOffset = offset;
    updateStepTiming(stepIndex, true); // Acquire lock for single step update
}

void Pattern::setStepSwingAffected(const size_t stepIndex, bool affected)
{
    if (stepIndex >= MAX_STEPS)
        return;
    this->steps[stepIndex].affectedBySwing = affected;
    updateStepTiming(stepIndex, true); // Acquire lock for single step update
}

void Pattern::setStepTrackId(const size_t stepIndex, uint32_t trackId)
{
    if (stepIndex >= MAX_STEPS)
        return;
    this->steps[stepIndex].trackId = trackId;
}

void Pattern::setStepNoteLength(const size_t stepIndex, NoteLength noteLength)
{
    if (stepIndex >= MAX_STEPS)
        return;
    this->steps[stepIndex].noteLength = noteLength;
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
      updateStepTiming(
          i); // Use default acquireLock=false since we already hold the lock
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
      if (this->steps[i].affectedBySwing) {
        updateStepTiming(
            i); // Use default acquireLock=false since we already hold the lock
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
      updateStepTiming(
          i); // Use default acquireLock=false since we already hold the lock
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
    if (this->steps[stepIndex].affectedBySwing && (stepIndex % 2) != 0) {
      finalTick += static_cast<int>(
          PPQN * this->swingAmount.load(std::memory_order_acquire));
    }

    // Apply micro-timing offset
    const float offset = this->steps[stepIndex].timingOffset;
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

void Pattern::updateStepTiming(const size_t stepIndex, bool acquireLock) {
  std::unique_ptr<std::lock_guard<std::mutex>> lock;
  if (acquireLock) {
    lock = std::make_unique<std::lock_guard<std::mutex>>(updateMutex);
  }

  size_t current = this->activeBuffer.load(std::memory_order_acquire);
  size_t inactive = 1 - current;

  // Work on inactive buffer
  auto &workingBuffer = triggerBuffers[inactive];
  // Copy maps from current buffer
  workingBuffer.tickToStep = triggerBuffers[current].tickToStep;
  workingBuffer.stepToTick = triggerBuffers[current].stepToTick;

  if (steps[stepIndex].enabled) {
    int finalTick = calculateStepTick(stepIndex);
    workingBuffer.addStep(finalTick, stepIndex);
  } else {
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
