# pragma once

#include "../Constants.h"
#include "../Identifiers.h"
#include "../JuceHeader.h"
#include "Step.h"
#include "Types.h"
#include "ValueTreeObject.h"

#include "../JuceHeader.h"
#include <atomic>
#include <map>
#include <mutex>
#include <vector>

namespace Sirkus::Core {

struct TriggerBuffer
{
    std::map<int, size_t> tickToStep; // Maps tick positions to step indices
    std::map<size_t, int> stepToTick; // Maps step indices to tick positions
    std::atomic<bool> dirty{false};

    void addStep(int tick, size_t stepIndex);
    void removeStep(size_t stepIndex);
    bool verifyIntegrity() const;

    // Allow copying of maps but reset dirty flag
    TriggerBuffer& operator=(const TriggerBuffer& other)
    {
        tickToStep = other.tickToStep;
        stepToTick = other.stepToTick;
        dirty.store(false);
        return *this;
    }
};

} // namespace Sirkus::Core
