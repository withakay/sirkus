#pragma once


#include <JuceHeader.h>


namespace Sirkus::ID {

#define DECLARE_ID(name) const juce::Identifier name (#name);

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
}

namespace Track {
DECLARE_ID(trackId)
DECLARE_ID(midiChannel)
DECLARE_ID(scaleMode)
}

#undef DECLARE_ID

} // namespace Sirkus::Core
