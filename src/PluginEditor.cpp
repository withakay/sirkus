#include "PluginProcessor.h"
#include "PluginEditor.h"

SirkusAudioProcessorEditor::SirkusAudioProcessorEditor(SirkusAudioProcessor& p)
    : AudioProcessorEditor(&p)
    , processorRef(p)
{
    // Play button setup
    addAndMakeVisible(playButton);
    playButton.setClickingTogglesState(true);
    playButton.onClick = [this]()
    {
        if (playButton.getToggleState())
        {
            processorRef.getTimingManager().start();
            playButton.setButtonText("Stop");
        }
        else
        {
            processorRef.getTimingManager().stop();
            playButton.setButtonText("Play");
        }
    };

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
    setSize(400, 200);

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
    playButton.setBounds(area.removeFromTop(30));
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
    auto& timing = processorRef.getTimingManager();

    if (auto pos = timing.getMusicalPosition())
    {
        juce::String posText;
        posText << "Position: Bar " << pos->bar
                << " | Beat " << pos->beat
                << " | Tick " << static_cast<int>(pos->tick);
        positionLabel.setText(posText, juce::dontSendNotification);
    }
    else
    {
        positionLabel.setText("Position: --", juce::dontSendNotification);
    }

    if (auto bpm = timing.getBpm())
    {
        bpmLabel.setText("BPM: " + juce::String(*bpm, 1), juce::dontSendNotification);
    }

    if (auto timeSig = timing.getTimeSignature())
    {
        timeSignatureLabel.setText(
            "Time Sig: " + juce::String(timeSig->first) + "/" + juce::String(timeSig->second),
            juce::dontSendNotification
        );
    }
}

void SirkusAudioProcessorEditor::updateTransportDisplay()
{
    bool isPlaying = processorRef.getTimingManager().isPlaying();
    if (playButton.getToggleState() != isPlaying)
    {
        playButton.setToggleState(isPlaying, juce::dontSendNotification);
        playButton.setButtonText(isPlaying ? "Stop" : "Play");
    }
}