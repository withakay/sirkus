#pragma once

#include "PluginProcessor.h"
#include "ui/GlobalControls.h"
#include "ui/PatternView.h"
#include "ui/SirkusLookAndFeel.h"
#include "ui/StepControls.h"
#include "ui/TransportControls.h"

#include <JuceHeader.h>

class SirkusAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::Timer,
                                   private Sirkus::UI::PatternView::Listener,
                                   private Sirkus::UI::StepControls::Listener,
                                   private Sirkus::UI::GlobalControls::Listener
{
public:
    explicit SirkusAudioProcessorEditor(SirkusAudioProcessor&);
    ~SirkusAudioProcessorEditor() override;
    SirkusAudioProcessorEditor(SirkusAudioProcessorEditor&&) noexcept;
    SirkusAudioProcessorEditor& operator=(SirkusAudioProcessorEditor&&) noexcept;

    void paint(juce::Graphics& /*g*/) override;
    void resized() override;
    void timerCallback() override;

private:
    SirkusAudioProcessor& processorRef;

    // Custom look and feel
    Sirkus::UI::SirkusLookAndFeel lookAndFeel;

    // UI Components
    Sirkus::UI::TransportControls transportControls;
    Sirkus::UI::PatternView patternView;
    Sirkus::UI::StepControls stepControls;
    Sirkus::UI::GlobalControls globalControls;

    juce::Label positionLabel;
    juce::Label bpmLabel;
    juce::Label timeSignatureLabel;

    // PatternView::Listener implementation
    void patternLengthChanged(const Sirkus::UI::PatternView* view, int newLength) noexcept override;
    void trackMidiChannelChanged(const Sirkus::UI::PatternView* view, int trackIndex, int newChannel) noexcept override;
    void stepStateChanged(const Sirkus::UI::PatternView* view, int trackIndex, int stepIndex) noexcept override;
    void stepSelectionChanged(const Sirkus::UI::PatternView* view) noexcept override;

    // StepControls::Listener implementation
    void noteValueChanged(Sirkus::UI::StepControls* controls, int newValue) override;
    void velocityChanged(Sirkus::UI::StepControls* controls, int newValue) override;
    void noteLengthChanged(Sirkus::UI::StepControls* controls, Sirkus::NoteLength newLength) override;

    // GlobalControls::Listener implementation
    void timeSignatureChanged(Sirkus::UI::GlobalControls* controls, int numerator, int denominator) override;
    void stepIntervalChanged(Sirkus::UI::GlobalControls* controls, Sirkus::StepInterval newInterval) override;

    void updatePositionDisplay();
    void updateTransportDisplay();
    void updatePatternView();
    void updateSelectedSteps();
    void updatePlaybackPosition();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SirkusAudioProcessorEditor)
};
