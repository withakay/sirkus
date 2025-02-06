

#include "TriggerBuffer.h"

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

} // namespace Sirkus::Core
