#include "Sequencer.h"
#include "Constants.h"
#include <algorithm>

namespace sirkus {

Sequencer::Sequencer() {
    // Create initial track
    addTrack();
}

Track* Sequencer::addTrack() {
    if (activeTrackCount >= MAX_TRACKS) {
        return nullptr;
    }
    
    const uint32_t trackId = generateTrackId();
    auto track = std::make_unique<Track>(trackId);
    
    // Apply global parameters
    track->setScale(globalScaleType, globalScaleRoot);
    track->getCurrentPattern()->setSwingAmount(globalSwing.load());
    
    // Store track
    tracks[activeTrackCount] = std::move(track);
    stepProcessor.addTrack(tracks[activeTrackCount].get());
    
    ++activeTrackCount;
    return tracks[activeTrackCount - 1].get();
}

bool Sequencer::removeTrack(uint32_t trackId) {
    // Can't remove the last track
    if (activeTrackCount <= 1) {
        return false;
    }
    
    // Find track with matching ID
    for (size_t i = 0; i < activeTrackCount; ++i) {
        if (tracks[i]->getId() == trackId) {
            // Remove from step processor
            stepProcessor.removeTrack(tracks[i].get());
            
            // Shift remaining tracks down
            for (size_t j = i; j < activeTrackCount - 1; ++j) {
                tracks[j] = std::move(tracks[j + 1]);
            }
            
            // Clear the last slot
            tracks[activeTrackCount - 1].reset();
            
            --activeTrackCount;
            return true;
        }
    }
    
    return false;
}

Track* Sequencer::getTrack(uint32_t trackId) {
    for (size_t i = 0; i < activeTrackCount; ++i) {
        if (tracks[i]->getId() == trackId) {
            return tracks[i].get();
        }
    }
    return nullptr;
}

const Track* Sequencer::getTrack(uint32_t trackId) const {
    for (size_t i = 0; i < activeTrackCount; ++i) {
        if (tracks[i]->getId() == trackId) {
            return tracks[i].get();
        }
    }
    return nullptr;
}

void Sequencer::prepare(double sampleRate) {
    currentSampleRate = sampleRate;
    timingManager.prepare(sampleRate);
}

void Sequencer::processBlock(juce::AudioPlayHead* playHead, int numSamples, juce::MidiBuffer& midiOut) {
    timingManager.processBlock(playHead, numSamples);
    
    const auto ppqPos = timingManager.getPpqPosition();
    const auto bpm = timingManager.getBpm();
    
    if (!ppqPos.has_value() || !bpm.has_value()) {
        return;
    }
    
    // Calculate tick range for this block
    const double samplesPerTick = (60.0 / *bpm / PPQN) * currentSampleRate;
    const double samplesToTicks = 1.0 / samplesPerTick;
    
    const uint64_t startTick = static_cast<uint64_t>(*ppqPos * PPQN);
    const uint64_t numTicks = static_cast<uint64_t>(numSamples * samplesToTicks);
    
    stepProcessor.processBlock(startTick, numTicks, midiOut);
}

void Sequencer::setGlobalSwing(float amount) {
    globalSwing.store(amount, std::memory_order_release);
    updateTrackSwing();
}

void Sequencer::setGlobalScale(Scale::Type type, uint8_t root) {
    globalScaleType = type;
    globalScaleRoot = root % 12;
    updateTrackScale();
}

void Sequencer::setGlobalCustomScale(const std::vector<uint8_t>& degrees, uint8_t root) {
    globalScaleType = Scale::Type::Custom;
    globalScaleRoot = root % 12;
    globalCustomDegrees = degrees;
    updateTrackScale();
}

uint32_t Sequencer::generateTrackId() {
    return nextTrackId++;
}

void Sequencer::updateTrackSwing() {
    const float amount = globalSwing.load(std::memory_order_acquire);
    for (size_t i = 0; i < activeTrackCount; ++i) {
        if (auto pattern = tracks[i]->getCurrentPattern()) {
            pattern->setSwingAmount(amount);
        }
    }
}

void Sequencer::updateTrackScale() {
    for (size_t i = 0; i < activeTrackCount; ++i) {
        if (globalScaleType == Scale::Type::Custom) {
            tracks[i]->setCustomScale(globalCustomDegrees, globalScaleRoot);
        } else {
            tracks[i]->setScale(globalScaleType, globalScaleRoot);
        }
    }
}

} // namespace sirkus