#include "Track.h"
#include "Step.h"
#include "Scale.h"
#include "Pattern.h"

#include <algorithm>
#include <cstdint>

namespace sirkus {

Track::Track(const uint32_t id) : trackId(id), currentPattern(std::make_unique<Pattern>())
{
}

void Track::setPattern(std::unique_ptr<Pattern> newPattern)
{
    currentPattern = std::move(newPattern);
}

void Track::setScale(Scale::Type type, uint8_t root)
{
    scale = Scale(type, root);
}

void Track::setCustomScale(const std::vector<uint8_t>& degrees, uint8_t root)
{
    scale = Scale(degrees, root);
}

uint8_t Track::quantizeNote(uint8_t note) const
{
    switch (scaleMode.load())
    {
        case ScaleMode::Off:
            return note;
        case ScaleMode::QuantizeUp:
            return scale.quantizeUp(note);
        case ScaleMode::QuantizeDown:
            return scale.quantizeDown(note);
        case ScaleMode::QuantizeRandom:
            return scale.quantizeRandom(note);
        default:
            return note;
    }
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

} // namespace sirkus
