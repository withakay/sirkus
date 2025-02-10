#include "Step.h"
#include "Types.h"

namespace Sirkus::Core {

Step::Step(ValueTree parentState, UndoManager& undoManagerToUse, int index)
        : ValueTreeObject(parentState, ID::step, undoManagerToUse, index)
{
    // Initialize default properties
    setProperty(props::enabled, false);
    setProperty(props::note, uint8_t(60));
    setProperty(props::velocity, uint8_t(100));
    setProperty(props::probability, 1.0f);
    setProperty(props::timingOffset, 0.0f);
    setProperty(props::affectedBySwing, true);
    setProperty(props::triggerTick, 0);
    setProperty(props::trackId, uint32_t(0));
    setProperty(props::noteLength, TimeDivision::SixteenthNote);
}

Step::Step(ValueTree existingState, UndoManager& undoManagerToUse, bool useExistingState)
        : ValueTreeObject(existingState, undoManagerToUse)
{
    // Need a parameter to avoid ambiguity with the ValueTreeObject constructor
    SIRKUS_UNUSED(useExistingState);
}


} // namespace Sirkus::Core
