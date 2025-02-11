#include "Pattern.h"

#include "../Constants.h"
#include "../Identifiers.h"
#include "../JuceHeader.h"
#include "Step.h"
#include "Types.h"
#include "ValueTreeObject.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include "juce_data_structures/juce_data_structures.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>

namespace Sirkus::Core {

using namespace Sirkus::Constants;

Pattern::Pattern(ValueTree parentState, UndoManager& undoManagerToUse)
        : ValueTreeObject(parentState, ID::pattern, undoManagerToUse)
        , props{}
{
    // Initialize default properties
    setLength(16);
    setSwingAmount(0.0f);
    setStepInterval(TimeDivision::SixteenthNote);

    // Initialize steps
    for (size_t i = 0; i < getLength(); ++i)
    {
        ensureStepExists(i);
        initializeStepTiming(i);
    }
}

void Pattern::ensureStepExists(const size_t stepIndex)
{
    if (stepIndex >= getLength())
        throw std::out_of_range("Step index out of range");

    if (!state.getChild(static_cast<int>(stepIndex)).isValid())
    {
        // Creating a new step ensures the values are added to
        // the ValueTree
        steps[stepIndex] = std::make_unique<Step>(state, undoManager, static_cast<int>(stepIndex));
    }
}

Step& Pattern::getStep(const size_t stepIndex) const
{
    return *steps[stepIndex];
}

bool Pattern::isStepEnabled(const size_t stepIndex) const
{
    return steps[stepIndex]->isEnabled();
}

void Pattern::setStepEnabled(const size_t stepIndex, const bool enabled)
{
    // NOLINTNEXTLINE
    DBG("Pattern::setStepEnabled: " << stepIndex << " -> " << std::to_string(enabled));

    steps[stepIndex]->setEnabled(enabled);
}

void Pattern::setStepNote(const size_t stepIndex, uint8_t note) const
{
    // NOLINTNEXTLINE
    DBG("Pattern::setStepNote: " << stepIndex << " -> " << std::to_string(note));

    steps[stepIndex]->setNote(note);
}

void Pattern::setStepVelocity(const size_t stepIndex, uint8_t velocity) const
{
    steps[stepIndex]->setVelocity(velocity);
}

void Pattern::setStepProbability(const size_t stepIndex, float probability) const
{
    steps[stepIndex]->setProbability(probability);
}

void Pattern::setStepOffset(const size_t stepIndex, const float offset)
{
    steps[stepIndex]->setTimingOffset(offset);
}

void Pattern::setStepSwingAffected(const size_t stepIndex, const bool affected)
{
    steps[stepIndex]->setAffectedBySwing(affected);
}

void Pattern::setStepTrackId(const size_t stepIndex, const uint32_t trackId)
{
    steps[stepIndex]->setTrackId(trackId);
}

void Pattern::setStepNoteLength(const size_t stepIndex, TimeDivision length)
{
    steps[stepIndex]->setNoteLength(length);
}

int Pattern::getStepStartTick(const size_t stepIndex) const
{
    return steps[stepIndex]->getTriggerTick();
}

int Pattern::calculateStepTick(size_t stepIndex) const
{
    if (stepIndex >= MAX_STEPS)
        return 0;

    // Calculate base tick using step interval
    const int gridSpacing = getStepInterval();
    const int baseTick = static_cast<int>(stepIndex) * gridSpacing;
    int finalTick = baseTick;

    // Apply swing if applicable
    if (steps[stepIndex]->isAffectedBySwing() && (stepIndex % 2) != 0)
    {
        finalTick += static_cast<int>(PPQN * getSwingAmount());
    }

    // Apply micro-timing offset
    const float offset = steps[stepIndex]->getTimingOffset();
    const int tickOffset = static_cast<int>(PPQN * offset);

    // Handle wrapping for negative offsets
    const int gridLength = static_cast<int>(getLength());
    const int patternLengthTicks = gridLength * gridSpacing;

    return (finalTick + tickOffset + patternLengthTicks) % patternLengthTicks;
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
