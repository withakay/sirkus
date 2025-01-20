#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class SirkusAudioProcessorEditor : public juce::AudioProcessorEditor,
                                  public juce::Timer
{
public:
    explicit SirkusAudioProcessorEditor(SirkusAudioProcessor&);
    ~SirkusAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    SirkusAudioProcessor& processorRef;

    juce::TextButton playButton {"Play"};
    juce::Label positionLabel;
    juce::Label bpmLabel;
    juce::Label timeSignatureLabel;

    void updatePositionDisplay();
    void updateTransportDisplay();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SirkusAudioProcessorEditor)
};