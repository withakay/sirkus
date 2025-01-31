#pragma once

#ifndef SIRKUS_IDENTIFIERS_H
#define SIRKUS_IDENTIFIERS_H

#include "JuceHeader.h"

namespace Sirkus::ID {

#define DECLARE_ID(name) const juce::Identifier name(#name);

DECLARE_ID(ROOT)
DECLARE_ID(sequencer)
DECLARE_ID(track)
DECLARE_ID(step)
DECLARE_ID(pattern)

namespace Sequencer {
DECLARE_ID(swingAmount)
}

namespace InternalTransport {
DECLARE_ID(bpm)
DECLARE_ID(ppqPosition)
DECLARE_ID(playing)
DECLARE_ID(timeSigNumerator)
DECLARE_ID(timeSigDenominator)
} // namespace InternalTransport

namespace Track {
DECLARE_ID(trackId)
DECLARE_ID(midiChannel)
DECLARE_ID(scaleMode)
} // namespace Track

namespace Pattern {
DECLARE_ID(length)
DECLARE_ID(swingAmount)
DECLARE_ID(stepInterval)
} // namespace Pattern

namespace Step {
DECLARE_ID(enabled)
DECLARE_ID(note)
DECLARE_ID(velocity)
DECLARE_ID(probability)
DECLARE_ID(timingOffset)
DECLARE_ID(affectedBySwing)
DECLARE_ID(triggerTick)
DECLARE_ID(trackId)
DECLARE_ID(noteLength)
} // namespace Step

#undef DECLARE_ID

} // namespace Sirkus::ID

#endif // SIRKUS_IDENTIFIERS_H
