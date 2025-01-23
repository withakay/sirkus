#pragma once

#include "Types.h"
#include <JuceHeader.h>
#include <vector>
#include <memory>

namespace sirkus {

// Forward declarations
class Track;
struct Step;

class StepProcessor {
public:
    StepProcessor();
    
    // Track registration
    void addTrack(Track* track);
    void removeTrack(Track* track);
    
    // Process all tracks and generate MIDI output
    void processBlock(uint64_t startTick, uint64_t numTicks, juce::MidiBuffer& midiOut);
    
    // Future: Modulation processing
    // void processModulations();
    // void addModulationRoute(uint32_t sourceTrack, uint32_t targetTrack, ModulationType type);
    
private:
    std::vector<Track*> tracks;
    
    // Helper methods
    void processStep(const Step& step, uint64_t triggerTick, uint64_t startTick, uint64_t numTicks, juce::MidiBuffer& midiOut);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepProcessor)
};

} // namespace sirkus