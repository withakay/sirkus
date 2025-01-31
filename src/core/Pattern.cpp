#include "Pattern.h"

#include <cassert>
#include <cstdint>

namespace Sirkus::Core {

using namespace Sirkus::Constants;

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

    if (!std::ranges::all_of(tickToStep, [this](const auto& pair) {
            const auto& [tick, step] = pair;
            auto it = stepToTick.find(step);
            return it != stepToTick.end() && it->second == tick;
        }))
    {
        return false;
    }
    return true;
}

Pattern::Pattern(ValueTree parentState, UndoManager& undoManagerToUse)
        : ValueTreeObject(parentState, ID::pattern, undoManagerToUse)
{
    // Initialize default properties
    setProperty(props::length, 16);
    setProperty(props::swingAmount, 0.0f);
    setProperty(props::stepInterval, StepInterval::Quarter);

    // Initialize steps
    for (size_t i = 0; i < 16; ++i)
    {
        ensureStepExists(i);
        initializeStepTiming(i);
    }
}

void Pattern::ensureStepExists(size_t stepIndex)
{
    if (stepIndex >= MAX_STEPS)
        return;

    if (!state.getChild(static_cast<int>(stepIndex)).isValid())
    {
        Step step(state, undoManager);
    }
}

Step Pattern::getStepObject(size_t stepIndex) const
{
    if (stepIndex >= MAX_STEPS)
        return Step(ValueTree(ID::step), undoManager);

    return Step(state.getChild(static_cast<int>(stepIndex)), undoManager);
}

bool Pattern::isStepEnabled(size_t stepIndex) const
{
    if (stepIndex >= MAX_STEPS)
        return false;

    auto step = getStepObject(stepIndex);
    return step.isEnabled();
}

void Pattern::setStepEnabled(const size_t stepIndex, const bool enabled)
{
    DBG("Pattern::setStepEnabled: " << stepIndex << " -> " << std::to_string(enabled));
    if (stepIndex >= MAX_STEPS)
        return;

    ensureStepExists(stepIndex);
    auto step = getStepObject(stepIndex);
    step.setEnabled(enabled);
    updateStepTiming(stepIndex, true);
}

void Pattern::setStepNote(const size_t stepIndex, uint8_t note)
{
    DBG("Pattern::setStepNote: " << stepIndex << " -> " << std::to_string(note));
    if (stepIndex >= MAX_STEPS)
        return;

    ensureStepExists(stepIndex);
    auto step = getStepObject(stepIndex);
    step.setNote(note);
}

void Pattern::setStepVelocity(const size_t stepIndex, uint8_t velocity)
{
    if (stepIndex >= MAX_STEPS)
        return;

    ensureStepExists(stepIndex);
    auto step = getStepObject(stepIndex);
    step.setVelocity(velocity);
}

void Pattern::setStepProbability(const size_t stepIndex, float probability)
{
    if (stepIndex >= MAX_STEPS)
        return;

    ensureStepExists(stepIndex);
    auto step = getStepObject(stepIndex);
    step.setProbability(probability);
}

void Pattern::setStepOffset(const size_t stepIndex, float offset)
{
    if (stepIndex >= MAX_STEPS)
        return;

    ensureStepExists(stepIndex);
    auto step = getStepObject(stepIndex);
    step.setTimingOffset(offset);
    updateStepTiming(stepIndex, true);
}

void Pattern::setStepSwingAffected(const size_t stepIndex, bool affected)
{
    if (stepIndex >= MAX_STEPS)
        return;

    ensureStepExists(stepIndex);
    auto step = getStepObject(stepIndex);
    step.setAffectedBySwing(affected);
    updateStepTiming(stepIndex, true);
}

void Pattern::setStepTrackId(const size_t stepIndex, uint32_t trackId)
{
    if (stepIndex >= MAX_STEPS)
        return;

    ensureStepExists(stepIndex);
    auto step = getStepObject(stepIndex);
    step.setTrackId(trackId);
}

void Pattern::setStepNoteLength(const size_t stepIndex, NoteLength length)
{
    if (stepIndex >= MAX_STEPS)
        return;

    ensureStepExists(stepIndex);
    auto step = getStepObject(stepIndex);
    step.setNoteLength(length);
}

int Pattern::calculateStepTick(const size_t stepIndex) const
{
    if (stepIndex >= MAX_STEPS)
        return 0;

    auto step = getStepObject(stepIndex);

    // Calculate base tick using step interval
    const int gridSpacing = stepIntervalToTicks(getProperty(props::stepInterval));
    const int baseTick = static_cast<int>(stepIndex) * gridSpacing;
    int finalTick = baseTick;

    // Apply swing if applicable
    if (step.isAffectedBySwing() && (stepIndex % 2) != 0)
    {
        finalTick += static_cast<int>(PPQN * getProperty(props::swingAmount));
    }

    // Apply micro-timing offset
    const float offset = step.getTimingOffset();
    const int tickOffset = static_cast<int>(PPQN * offset);

    // Handle wrapping for negative offsets
    const int gridLength = static_cast<int>(getProperty(props::length));
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
    auto step = getStepObject(stepIndex);
    const int noteLengthTicks = step.getNoteLengthInTicks();

    // Handle pattern wrapping
    const int gridSpacing = stepIntervalToTicks(getProperty(props::stepInterval));
    const int gridLength = static_cast<int>(getProperty(props::length));
    const int patternLengthTicks = gridLength * gridSpacing;
    return (startTick + noteLengthTicks) % patternLengthTicks;
}

void Pattern::updateStepTiming(const size_t stepIndex, bool acquireLock)
{
    if (stepIndex >= MAX_STEPS)
        return;

    std::unique_ptr<std::lock_guard<std::mutex>> lock;
    if (acquireLock)
    {
        lock = std::make_unique<std::lock_guard<std::mutex>>(updateMutex);
    }

    size_t current = this->activeBuffer.load(std::memory_order_acquire);
    size_t inactive = 1 - current;

    // Work on inactive buffer
    auto& workingBuffer = triggerBuffers[inactive];
    // Copy maps from current buffer
    workingBuffer.tickToStep = triggerBuffers[current].tickToStep;
    workingBuffer.stepToTick = triggerBuffers[current].stepToTick;

    if (isStepEnabled(stepIndex))
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

void Pattern::initializeStepTiming(const size_t stepIndex)
{
    if (stepIndex >= MAX_STEPS)
        return;

    if (isStepEnabled(stepIndex))
    {
        int finalTick = calculateStepTick(stepIndex);
        triggerBuffers[0].addStep(finalTick, stepIndex);
    }

    // Mark as initialized
    triggerBuffers[0].dirty.store(true, std::memory_order_release);
    // Set buffer 0 as active
    this->activeBuffer.store(0, std::memory_order_release);
}

const std::map<int, size_t>& Pattern::getTriggerMap() const
{
    return triggerBuffers[this->activeBuffer.load(std::memory_order_acquire)].tickToStep;
}

} // namespace Sirkus::Core
