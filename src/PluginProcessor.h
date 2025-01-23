/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "Sequencer.h"
#include <JuceHeader.h>

//==============================================================================
/**
 */
class SirkusAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    SirkusAudioProcessor();
    ~SirkusAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Timing control methods
    void setStandaloneBpm(double bpm);
    void startStandalonePlayback();
    void stopStandalonePlayback();
    bool isStandalonePlaying() const;
    bool isInStandaloneMode() const;

    // Host sync control
    void setHostSyncEnabled(bool enabled) { sequencer.getTimingManager().setHostSyncEnabled(enabled); }
    [[nodiscard]] bool isHostSyncEnabled() const { return sequencer.getTimingManager().isHostSyncEnabled(); }

    // Direct access to sequencer
    sirkus::Sequencer& getSequencer() { return sequencer; }
    const sirkus::Sequencer& getSequencer() const { return sequencer; }

private:
    sirkus::Sequencer sequencer;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SirkusAudioProcessor)
};
