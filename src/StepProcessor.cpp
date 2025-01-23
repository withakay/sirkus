#include "StepProcessor.h"
#include "Track.h"
#include "Constants.h"
#include <algorithm>

namespace sirkus {

StepProcessor::StepProcessor() {
}

void StepProcessor::addTrack(Track* track) {
    if (!track) return;
    
    // Check if track already exists
    auto it = std::find(tracks.begin(), tracks.end(), track);
    if (it == tracks.end()) {
        tracks.push_back(track);
    }
}

void StepProcessor::removeTrack(Track* track) {
    auto it = std::find(tracks.begin(), tracks.end(), track);
    if (it != tracks.end()) {
        tracks.erase(it);
    }
}

void StepProcessor::processBlock(uint64_t startTick, uint64_t numTicks, juce::MidiBuffer& midiOut) {
    // Collect all active steps from all tracks
    for (const auto* track : tracks) {
        if (!track) continue;
        
        auto activeSteps = track->getActiveSteps(startTick, numTicks);
        
        // Process each active step
        for (const auto& [triggerTick, step] : activeSteps) {
            if (juce::Random::getSystemRandom().nextFloat() <= step->probability) {
                processStep(*step, triggerTick, startTick, numTicks, midiOut);
            }
        }
    }
}

void StepProcessor::processStep(const Step& step, uint64_t triggerTick, uint64_t startTick, uint64_t numTicks, juce::MidiBuffer& midiOut) {
    // Find the track that owns this step
    const uint32_t stepTrackId = step.trackId.load();
    auto it = std::find_if(tracks.begin(), tracks.end(),
                          [stepTrackId](const Track* t) { 
                              return t && t->getId() == stepTrackId; 
                          });
    
    if (it == tracks.end()) return;
    
    const Track* track = *it;
    const int noteOnOffset = static_cast<int>((triggerTick - startTick));
    
    // Apply scale quantization
    uint8_t finalNote = track->quantizeNote(step.note);
    
    const uint8_t channel = track->getMidiChannel();
    const uint8_t velocity = step.velocity;
    
    // Calculate note-off position based on note length
    const uint64_t noteLengthTicks = step.getNoteLengthInTicks();
    const uint64_t noteOffTick = triggerTick + noteLengthTicks;
    const int noteOffOffset = static_cast<int>((noteOffTick - startTick));
    
    // Add note-on event if it falls within this block
    if (noteOnOffset >= 0 && noteOnOffset < static_cast<int>(numTicks)) {
        midiOut.addEvent(juce::MidiMessage::noteOn(channel, finalNote, velocity), noteOnOffset);
    }
    
    // Add note-off event if it falls within this block
    if (noteOffOffset >= 0 && noteOffOffset < static_cast<int>(numTicks)) {
        midiOut.addEvent(juce::MidiMessage::noteOff(channel, finalNote), noteOffOffset);
    }
}

} // namespace sirkus
