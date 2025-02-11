#include "Step.h"
#include "Types.h"

namespace Sirkus::Core {

Step::Step(ValueTree parentState, UndoManager& undoManagerToUse, int index)
    : ValueTreeObject(parentState, ID::step, undoManagerToUse, index)
      , props{}
{
    // Initialize default properties via props{} call

}

Step::Step(ValueTree existingState, UndoManager& undoManagerToUse, bool useExistingState)
    : ValueTreeObject(existingState, undoManagerToUse)
{
    // Need a parameter to avoid ambiguity with the ValueTreeObject constructor
    SIRKUS_UNUSED(useExistingState);
}


} // namespace Sirkus::Core
