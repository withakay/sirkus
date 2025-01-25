#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "ui/TransportControls.h"

namespace sirkus {

class SirkusAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    explicit SirkusAudioProcessorEditor(sirkus::SirkusAudioProcessor&);
    ~SirkusAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    sirkus::SirkusAudioProcessor& processorRef;

    ui::TransportControls transportControls;
    juce::Label positionLabel;
    juce::Label bpmLabel;
    juce::Label timeSignatureLabel;

    void updatePositionDisplay();
    void updateTransportDisplay();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SirkusAudioProcessorEditor)
};

} // namespace sirkus
