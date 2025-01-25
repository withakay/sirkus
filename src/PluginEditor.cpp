#include <JuceHeader.h>

#include "PluginEditor.h"
#include "Sequencer.h"
#include "TimingManager.h"

SirkusAudioProcessorEditor::SirkusAudioProcessorEditor(SirkusAudioProcessor& p)
    : AudioProcessorEditor(&p)
      , processorRef(p)
      , transportControls(p)
{
    addAndMakeVisible(transportControls);

    // Labels setup
    addAndMakeVisible(positionLabel);
    positionLabel.setJustificationType(juce::Justification::left);
    positionLabel.setText("Position: --", juce::dontSendNotification);

    addAndMakeVisible(bpmLabel);
    bpmLabel.setJustificationType(juce::Justification::left);
    bpmLabel.setText("BPM: --", juce::dontSendNotification);

    addAndMakeVisible(timeSignatureLabel);
    timeSignatureLabel.setJustificationType(juce::Justification::left);
    timeSignatureLabel.setText("Time Sig: --", juce::dontSendNotification);

    // Size setup
    setSize(900, 500);

    // Start timer for updates
    startTimerHz(30); // 30 fps update rate
}

SirkusAudioProcessorEditor::~SirkusAudioProcessorEditor()
{
    stopTimer();
}

void SirkusAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void SirkusAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10);

    // Layout controls vertically
    transportControls.setBounds(area.removeFromTop(70));
    area.removeFromTop(10); // spacing

    positionLabel.setBounds(area.removeFromTop(25));
    area.removeFromTop(5);

    bpmLabel.setBounds(area.removeFromTop(25));
    area.removeFromTop(5);

    timeSignatureLabel.setBounds(area.removeFromTop(25));
}

void SirkusAudioProcessorEditor::timerCallback()
{
    updatePositionDisplay();
    updateTransportDisplay();
}

void SirkusAudioProcessorEditor::updatePositionDisplay()
{
    const auto& sequencer = processorRef.getSequencer();
    const auto& timing = sequencer.getTimingManager();

    if (const auto pos = timing.getMusicalPosition())
    {
        juce::String posText;
        posText << "Position: Bar " << pos->bar << " | Beat " << pos->beat << " | Tick " << static_cast<int>(pos->tick);
        positionLabel.setText(posText, juce::dontSendNotification);
    }
    else
    {
        positionLabel.setText("Position: --", juce::dontSendNotification);
    }

    if (const auto bpm = timing.getBpm())
    {
        bpmLabel.setText("BPM: " + juce::String(*bpm, 1), juce::dontSendNotification);
    }

    if (const auto timeSig = timing.getTimeSignature())
    {
        timeSignatureLabel.setText(
            "Time Sig: " + juce::String(timeSig->first) + "/" + juce::String(timeSig->second),
            juce::dontSendNotification);
    }
}

void SirkusAudioProcessorEditor::updateTransportDisplay()
{
    // Transport state is now handled by TransportControls component
}
