/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

SirkusAudioProcessor::SirkusAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
    #if !JucePlugin_IsMidiEffect
        #if !JucePlugin_IsSynth
              .withInput ("Input", juce::AudioChannelSet::stereo(), true)
        #endif
              .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
    #endif
      )
#endif
{
}

SirkusAudioProcessor::~SirkusAudioProcessor() = default;

void SirkusAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    timingManager.prepare(sampleRate);
    midiGenerator.prepare(sampleRate);
}

void SirkusAudioProcessor::releaseResources()
{
}

void SirkusAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    const auto numSamples = buffer.getNumSamples();
    timingManager.processBlock(getPlayHead(), numSamples);

    midiMessages.clear();

    const auto ppqPos = timingManager.getPpqPosition();

    if (const auto bpm = timingManager.getBpm(); ppqPos.has_value() && bpm.has_value())
    {
        midiGenerator.generateEvents(*ppqPos, *bpm, numSamples, midiMessages);
    }
}

const juce::String SirkusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SirkusAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SirkusAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SirkusAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SirkusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SirkusAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SirkusAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SirkusAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SirkusAudioProcessor::getProgramName (int index)
{
    return {};
}

void SirkusAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

// Standalone control methods
void SirkusAudioProcessor::setStandaloneBpm(double bpm)
{
    timingManager.setBpm(bpm);
}

void SirkusAudioProcessor::startStandalonePlayback()
{
    timingManager.start();
}

void SirkusAudioProcessor::stopStandalonePlayback()
{
    timingManager.stop();
}

bool SirkusAudioProcessor::isStandalonePlaying() const
{
    return timingManager.isPlaying();
}

bool SirkusAudioProcessor::isInStandaloneMode() const
{
    return timingManager.isStandaloneMode();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SirkusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif




//==============================================================================
bool SirkusAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SirkusAudioProcessor::createEditor()
{
    return new SirkusAudioProcessorEditor (*this);
}

//==============================================================================
void SirkusAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SirkusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SirkusAudioProcessor();
}
