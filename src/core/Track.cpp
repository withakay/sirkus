#include "Track.h"
#include "Pattern.h"
#include "Step.h"

#include <algorithm>
#include <cstdint>

namespace Sirkus::Core {

Track::Track(const uint32_t id) : trackId(id), currentPattern(std::make_unique<Pattern>())
{
}

void Track::setPattern(std::unique_ptr<Pattern> newPattern)
{
    currentPattern = std::move(newPattern);
}

std::vector<std::pair<int, const Step*>> Track::getActiveSteps(int startTick, int numTicks) const
{
    std::vector<std::pair<int, const Step*>> activeSteps;
    if (!currentPattern)
        return activeSteps;

    const auto& triggers = currentPattern->getTriggerMap();

    // Find first trigger at or after startTick
    auto it = triggers.lower_bound(startTick);

    // Collect all triggers within this block
    while (it != triggers.end() && it->first < startTick + numTicks)
    {
        const auto stepIndex = it->second;
        const auto& step = currentPattern->getStep(stepIndex);

        // Only include enabled steps that pass probability check
        if (step.enabled)
        {
            activeSteps.emplace_back(it->first, &step);
        }

        ++it;
    }

    return activeSteps;
}

} // namespace Sirkus::Core
