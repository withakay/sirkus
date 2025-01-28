#pragma once

#include "PluginProcessor.h"
#include "ui/GlobalControls.h"
#include "ui/MidiEventLog.h"
#include "ui/SirkusLookAndFeel.h"
#include "ui/StepControls.h"
#include "ui/TrackPanel.h"
#include "ui/TransportControls.h"

#include <JuceHeader.h>

class SirkusAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::Timer,
                                   private Sirkus::UI::TrackPanel::Listener,
                                   private Sirkus::UI::StepControls::Listener,
                                   private Sirkus::UI::GlobalControls::Listener
{
public:
    explicit SirkusAudioProcessorEditor(SirkusAudioProcessor&);
    ~SirkusAudioProcessorEditor() override;

    void paint(juce::Graphics& /*g*/) override;
    void resized() override;
    void timerCallback() override;

private:
    SirkusAudioProcessor& processorRef;

    // Custom look and feel
    Sirkus::UI::SirkusLookAndFeel lookAndFeel;

    // UI Components
    Sirkus::UI::TransportControls transportControls;
    Sirkus::UI::TrackPanel trackPanel;
    Sirkus::UI::StepControls stepControls;
    Sirkus::UI::GlobalControls globalControls;
    Sirkus::UI::MidiEventLog midiEventLog;

    juce::Label positionLabel;
    juce::Label bpmLabel;
    juce::Label timeSignatureLabel;

    // TrackPanel::Listener implementation
    void trackMidiChannelChanged(const Sirkus::UI::TrackPanel* panel, int trackIndex, int newChannel) noexcept override;
    void stepStateChanged(const Sirkus::UI::TrackPanel* panel, int trackIndex, int stepIndex) noexcept override;
    void stepSelectionChanged(const Sirkus::UI::TrackPanel* panel) noexcept override;
    void pageChanged(const Sirkus::UI::TrackPanel* panel, int trackIndex, int newPage) noexcept override;
    void patternLengthChanged(const Sirkus::UI::TrackPanel* panel, int trackIndex, int newLength) noexcept override;

    // StepControls::Listener implementation
    void noteValueChanged(Sirkus::UI::StepControls* controls, int newValue) override;
    void velocityChanged(Sirkus::UI::StepControls* controls, int newValue) override;
    void noteLengthChanged(Sirkus::UI::StepControls* controls, Sirkus::Core::NoteLength newLength) override;

    // GlobalControls::Listener implementation
    void timeSignatureChanged(Sirkus::UI::GlobalControls* controls, int numerator, int denominator) override;
    void stepIntervalChanged(Sirkus::UI::GlobalControls* controls, Sirkus::Core::StepInterval newInterval) override;

    void updatePositionDisplay();
    void updateTransportDisplay();
    void updateTrackPanel();
    void updateSelectedSteps();
    void updatePlaybackPosition();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SirkusAudioProcessorEditor)
};
