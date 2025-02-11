#include "Sequencer.h"

#include "../Constants.h"
#include "../JuceHeader.h"
#include "Pattern.h"
#include "StepProcessor.h"
#include "TimingManager.h"
#include "Track.h"

#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace Sirkus::Core {

Sequencer::Sequencer(ValueTree parentState, UndoManager& undoManagerToUse)
    : ValueTreeObject(parentState, ID::sequencer, undoManagerToUse)
      , props{}
{

    // Load any existing tracks from state tree
    // for (int i = 0; i < state.getNumChildren(); ++i)
    // {
    //     auto trackTree = state.getChild(i);
    //     if (trackTree.hasType(ID::track))
    //     {
    //         auto var = trackTree.getProperty(ID::Track::trackId);
    //         auto trackId = static_cast<uint32_t>(var.operator int());
    //         nextTrackId = std::max(nextTrackId, trackId + 1);
    //         tracks.emplace_back(trackTree, undoManager, trackId);
    //     }
    // }

    // Create initial track if none exist
    if (tracks.empty())
    {
        createTrack();
    }
}

uint32_t Sequencer::createTrack()
{
    if (getTrackCount() >= MAX_TRACKS)
    {
        return 0; // Return invalid trackId
    }

    const uint32_t trackId = generateTrackId();
    auto track = std::make_unique<Track>(state, undoManager, trackId);
    // Apply global swing to pattern
    Pattern& pattern = track->getCurrentPattern();
    pattern.setSwingAmount(getProperty(props.swingAmount));
    tracks.push_back(std::move(track));
    DBG("Sequencer::createTrack(); trackId=" << std::to_string(trackId));
    DBG("Pattern length: " << std::to_string(tracks.back()->getCurrentPattern().getLength()));
    for (size_t i = 0; i < pattern.getLength(); ++i)
    {
        auto& step = pattern.getStep(i);
        DBG("Step number: " << std::to_string(i));
        DBG("- Step note: " << std::to_string(step.getNote()));
    }
    return trackId;
}

bool Sequencer::removeTrack(uint32_t trackId)
{
    // Can't remove the last track
    if (getTrackCount() <= 1)
    {
        return false;
    }

    // Find track with matching ID
    auto trackTree = state.getChildWithProperty(ID::Track::trackId, static_cast<int>(trackId));
    if (trackTree.isValid())
    {
        state.removeChild(trackTree, &undoManager);
        // Remove from tracks vector
        std::erase_if(
            tracks,
            [trackId](std::unique_ptr<Track>& t) {
                return t->getId() == trackId;
            });
        return true;
    }

    return false;
}

Track& Sequencer::getTrack(const uint32_t trackId)
{
    // Try to find the track in our vector
    for (const auto& track : tracks)
    {
        if (track->getId() == trackId)
        {
            return *track;
        }
    }

    DBG("Internal State: " << state.toXmlString());
    throw std::runtime_error("Track not found for id: " + std::to_string(trackId));
}

std::vector<std::unique_ptr<Track>>& Sequencer::getTracks()
{
    return tracks;
}

Pattern& Sequencer::getCurrentPatternForTrack(uint32_t trackId)
{
    return getTrack(trackId).getCurrentPattern();
}

void Sequencer::prepare(const double sampleRate)
{
    currentSampleRate = sampleRate;
    timingManager.prepare(sampleRate);
}

void Sequencer::processBlock(const juce::AudioPlayHead* playHead, const int numSamples, juce::MidiBuffer& midiOut)
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
    for (const auto& track : getTracks())
    {
        auto activeSteps = track->getActiveSteps(startTick, numTicks);
        stepProcessor.processSteps(activeSteps, track->getTrackInfo(), globalScale, startTick, numTicks, midiOut);
    }
}

Scale::Type Sequencer::getScaleType() const
{
    return scaleType;
}

uint8_t Sequencer::getScaleRoot() const
{
    return scaleRoot;
}

const std::vector<uint8_t>& Sequencer::getGlobalCustomDegrees() const
{
    return globalCustomDegrees;
}

float Sequencer::getSwingAmount() const
{
    return getProperty(props.swingAmount);
}

void Sequencer::setSwingAmount(const float amount)
{
    setProperty(props.swingAmount, amount);
    updateTrackSwing();
}

void Sequencer::setScale(Scale::Type type, uint8_t root)
{
    scaleType = type;
    scaleRoot = root % 12;
    globalScale = Scale(type, root);
}

void Sequencer::setCustomScale(const std::vector<uint8_t>& degrees, uint8_t root)
{
    scaleType = Scale::Type::Custom;
    scaleRoot = root % 12;
    globalCustomDegrees = degrees;
    globalScale = Scale(degrees, root);
}

uint32_t Sequencer::generateTrackId()
{
    return nextTrackId++;
}

size_t Sequencer::getTrackCount() const
{
    size_t count = 0;
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        if (state.getChild(i).hasType(ID::track))
            count++;
    }
    return count;
}

TimingManager& Sequencer::getTimingManager()
{
    return timingManager;
}

void Sequencer::updateTrackSwing()
{
    const float amount = getProperty(props.swingAmount);
    for (auto& track : getTracks())
    {
        auto& pattern = track->getCurrentPattern();
        pattern.setSwingAmount(amount);
    }
}

} // namespace Sirkus::Core
