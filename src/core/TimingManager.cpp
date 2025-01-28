#include "TimingManager.h"

namespace Sirkus::Core {
TimingManager::TimingManager()
    : standaloneMode(false)
      , hostSyncEnabled(true)
{
}

void TimingManager::prepare(const double sampleRate)
{
    internalTransport.prepare(sampleRate);
}

void TimingManager::processBlock(const juce::AudioPlayHead* playHead, const int numSamples)
{
    if (playHead != nullptr && hostSyncEnabled)
    {
        if (auto pos = playHead->getPosition(); pos->getPpqPosition().hasValue() && pos->getBpm().hasValue())
        {
            standaloneMode = false;
            currentTiming = TimingInfo::fromPositionInfo(*pos);
            return;
        }
    }

    // Use internal transport if:
    // - No host is available
    // - Host sync is disabled
    // - Host doesn't provide required timing info
    standaloneMode = true;
    internalTransport.processBlock(numSamples);
    currentTiming = TimingInfo::fromInternalTransport(internalTransport);
}

std::optional<double> TimingManager::getPpqPosition() const
{
    return currentTiming.ppqPosition;
}

std::optional<double> TimingManager::getBpm() const
{
    return currentTiming.bpm;
}

std::optional<MusicalPosition> TimingManager::getMusicalPosition() const
{
    return currentTiming.musicalPosition;
}

std::optional<std::pair<int, int>> TimingManager::getTimeSignature() const
{
    return currentTiming.timeSignature;
}
} // namespace Sirkus::Core
