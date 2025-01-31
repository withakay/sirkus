#include "TransportControls.h"

namespace Sirkus::UI {

TransportControls::TransportControls(SirkusAudioProcessor& p)
    : processorRef(p)
{
    // Play button setup
    addAndMakeVisible(playButton);
    playButton.setClickingTogglesState(true);
    playButton.onClick = [this]() {
        if (playButton.getToggleState())
        {
            processorRef.getSequencer().getTimingManager().start();
            playButton.setButtonText("Stop");
        }
        else
        {
            processorRef.getSequencer().getTimingManager().stop();
            playButton.setButtonText("Play");
        }
    };

    // Host sync button setup
    addAndMakeVisible(hostSyncButton);
    hostSyncButton.setToggleState(processorRef.isHostSyncEnabled(), juce::dontSendNotification);
    hostSyncButton.onClick = [this]() { processorRef.setHostSyncEnabled(hostSyncButton.getToggleState()); };
}

void TransportControls::resized()
{
    auto area = getLocalBounds().reduced(10);

    // Layout controls vertically
    playButton.setBounds(area.removeFromTop(30));
    area.removeFromTop(10); // spacing

    hostSyncButton.setBounds(area.removeFromTop(30));
}

} // namespace Sirkus::UI
