#include "Track.h"

#include "Pattern.h"
#include "Step.h"

#include <algorithm>
#include <cstdint>
#include <memory>

namespace Sirkus::Core {

Track::Track(ValueTree parentState, UndoManager& undoManagerToUse, uint32_t id)
        : ValueTreeObject(parentState, ID::track, undoManagerToUse)
{
    setProperty(props::trackId, id);
    setProperty(props::midiChannel, static_cast<uint8_t>(1));
    setProperty(props::scaleMode, ScaleMode::Off);

    // Create initial pattern
    ensurePatternExists();
}

void Track::ensurePatternExists()
{
    currentPattern = std::make_unique<Pattern>(state, undoManager);
}

Pattern& Track::getCurrentPattern() const
{
    return *currentPattern;
}

std::vector<std::pair<int, const Step*>> Track::getActiveSteps(int startTick, int numTicks) const
{
    std::vector<std::pair<int, const Step*>> activeSteps;
    // const Pattern& patternRef = getCurrentPattern();
    const auto& triggers = getCurrentPattern().getTriggerMap();

    // Find first trigger at or after startTick
    auto it = triggers.lower_bound(startTick);

    // Collect all triggers within this block
    while (it != triggers.end() && it->first < startTick + numTicks)
    {
        const auto stepIndex = it->second;
        auto step = getCurrentPattern().getStep(stepIndex);

        // Only include enabled steps
        if (step.isEnabled())
        {
            activeSteps.emplace_back(it->first, &step);
        }

        ++it;
    }

    return activeSteps;
}

} // namespace Sirkus::Core
