#pragma once

#include "../PluginProcessor.h"
#include <JuceHeader.h>

namespace Sirkus::UI {

class TransportControls : public juce::Component
{
public:
    explicit TransportControls(SirkusAudioProcessor&);
    ~TransportControls() override = default;

    void resized() override;

private:
    SirkusAudioProcessor& processorRef;

    juce::TextButton playButton{"Play"};
    juce::ToggleButton hostSyncButton{"Sync to Host"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportControls)
};

} // namespace Sirkus::UI
