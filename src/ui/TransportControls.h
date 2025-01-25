#pragma once

#include "../PluginProcessor.h"
#include <JuceHeader.h>

namespace sirkus::ui {

class TransportControls : public juce::Component
{
public:
    explicit TransportControls(sirkus::SirkusAudioProcessor&);
    ~TransportControls() override = default;

    void resized() override;

private:
    sirkus::SirkusAudioProcessor& processorRef;

    juce::TextButton playButton{"Play"};
    juce::ToggleButton hostSyncButton{"Sync to Host"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportControls)
};

} // namespace sirkus::ui