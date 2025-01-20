/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MidiEventGenerator.h"
#include "TimingManager.h"

//==============================================================================
/**
*/
class SirkusAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SirkusAudioProcessor();
    ~SirkusAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Standalone timing control methods
    void setStandaloneBpm(double bpm);
    void startStandalonePlayback();
    void stopStandalonePlayback();
    bool isStandalonePlaying() const;
    bool isInStandaloneMode() const;

    // Direct access to timing manager
    TimingManager& getTimingManager() { return timingManager; }
    const TimingManager& getTimingManager() const { return timingManager; }

private:
    MidiEventGenerator midiGenerator;
    TimingManager timingManager;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SirkusAudioProcessor)
};
