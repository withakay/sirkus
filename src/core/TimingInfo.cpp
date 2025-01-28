#include "TimingInfo.h"

#include "../Constants.h"
#include "InternalTransport.h"

namespace Sirkus::Core {

using namespace Sirkus::Constants;

TimingInfo TimingInfo::fromPositionInfo(const juce::AudioPlayHead::PositionInfo& pos)
{
    TimingInfo info;

    if (auto ppq = pos.getPpqPosition())
        info.ppqPosition = *ppq;

    if (auto tempo = pos.getBpm())
        info.bpm = *tempo;

    if (auto timeSig = pos.getTimeSignature())
        info.timeSignature = std::make_pair(timeSig->numerator, timeSig->denominator);

    info.isPlaying = pos.getIsPlaying();

    // Calculate musical position if we have PPQ and time signature
    if (info.ppqPosition && info.timeSignature)
    {
        MusicalPosition position;
        const double beatsPerBar = info.timeSignature->first * (4.0 / info.timeSignature->second);
        const double totalBeats = *info.ppqPosition * (info.timeSignature->second / 4.0);

        position.bar = static_cast<int>(totalBeats / beatsPerBar) + 1;
        position.beat = static_cast<int>(std::fmod(totalBeats, beatsPerBar)) + 1;
        position.tick = std::fmod(totalBeats, 1.0) * PPQN;

        info.musicalPosition = position;
    }

    return info;
}

TimingInfo TimingInfo::fromInternalTransport(const InternalTransport& transport)
{
    return transport.getTimingInfo();
}
} // namespace Sirkus::Core
