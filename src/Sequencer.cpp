#include "Sequencer.h"
#include "Constants.h"
#include <algorithm>
#include <vector>

namespace sirkus {

Sequencer::Sequencer()
{
    // Create initial track
    addTrack();
}

Track* Sequencer::addTrack()
{
  if (tracks.size() >= MAX_TRACKS) {
    return nullptr;
  }

    const uint32_t trackId = generateTrackId();
    auto track = std::make_unique<Track>(trackId);

    // Apply global swing to pattern
    Pattern *pattern = track->getCurrentPattern();
    pattern->setSwingAmount(globalSwing);

    // Store track and get raw pointer before moving
    auto *rawTrack = track.get();
    tracks.push_back(std::move(track));
    return rawTrack;
}

bool Sequencer::removeTrack(uint32_t trackId)
{
    // Can't remove the last track
    if (tracks.size() <= 1) {
      return false;
    }

    // Find track with matching ID
    auto it = std::find_if(
        tracks.begin(), tracks.end(),
        [trackId](const auto &track) { return track->getId() == trackId; });

    if (it != tracks.end()) {
      tracks.erase(it);
      return true;
    }

    return false;
}

Track* Sequencer::getTrack(uint32_t trackId)
{
  auto it =
      std::find_if(tracks.begin(), tracks.end(), [trackId](const auto &track) {
        return track->getId() == trackId;
      });
  return it != tracks.end() ? it->get() : nullptr;
}

const Track* Sequencer::getTrack(uint32_t trackId) const
{
  auto it =
      std::find_if(tracks.begin(), tracks.end(), [trackId](const auto &track) {
        return track->getId() == trackId;
      });
  return it != tracks.end() ? it->get() : nullptr;
}

void Sequencer::prepare(double sampleRate)
{
    currentSampleRate = sampleRate;
    timingManager.prepare(sampleRate);
}

void Sequencer::processBlock(juce::AudioPlayHead* playHead, int numSamples, juce::MidiBuffer& midiOut)
{
    timingManager.processBlock(playHead, numSamples);

    const auto ppqPos = timingManager.getPpqPosition();
    const auto bpm = timingManager.getBpm();

    if (!ppqPos.has_value() || !bpm.has_value())
    {
        return;
    }

    // Calculate tick range for this block
    const double samplesPerTick = (60.0 / *bpm / PPQN) * currentSampleRate;
    const double samplesToTicks = 1.0 / samplesPerTick;

    const int startTick = static_cast<int>(*ppqPos * PPQN);
    const int numTicks = static_cast<int>(numSamples * samplesToTicks);

    // Process each track's steps
    for (const auto &track : tracks) {
      auto activeSteps = track->getActiveSteps(startTick, numTicks);
      stepProcessor.processSteps(activeSteps, track->getTrackInfo(),
                                 globalScale, startTick, numTicks, midiOut);
    }
}

void Sequencer::setGlobalSwing(const float amount) {
  globalSwing = amount;
  updateTrackSwing();
}

void Sequencer::setGlobalScale(Scale::Type type, uint8_t root)
{
    globalScaleType = type;
    globalScaleRoot = root % 12;
    globalScale = Scale(type, root);
}

void Sequencer::setGlobalCustomScale(const std::vector<uint8_t>& degrees, uint8_t root)
{
    globalScaleType = Scale::Type::Custom;
    globalScaleRoot = root % 12;
    globalCustomDegrees = degrees;
    globalScale = Scale(degrees, root);
}

uint32_t Sequencer::generateTrackId()
{
    return nextTrackId++;
}

void Sequencer::updateTrackSwing() const
{
  const float amount = globalSwing;
  for (const auto &track : tracks) {
    if (auto *pattern = track->getCurrentPattern()) {
      pattern->setSwingAmount(amount);
    }
  }
}

} // namespace sirkus
