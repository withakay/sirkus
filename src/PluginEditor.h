#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "ui/GlobalControls.h"
#include "ui/PatternView.h"
#include "ui/SirkusLookAndFeel.h"
#include "ui/StepControls.h"
#include "ui/TransportControls.h"


class SirkusAudioProcessorEditor
    : public juce::AudioProcessorEditor,
      public juce::Timer,
      private sirkus::ui::PatternView::Listener,
      private sirkus::ui::StepControls::Listener,
      private sirkus::ui::GlobalControls::Listener {
public:
    explicit SirkusAudioProcessorEditor(SirkusAudioProcessor&);
    ~SirkusAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    SirkusAudioProcessor& processorRef;

    // Custom look and feel
    sirkus::ui::SirkusLookAndFeel lookAndFeel;

    // UI Components
    sirkus::ui::TransportControls transportControls;
    sirkus::ui::PatternView patternView;
    sirkus::ui::StepControls stepControls;
    sirkus::ui::GlobalControls globalControls;

    juce::Label positionLabel;
    juce::Label bpmLabel;
    juce::Label timeSignatureLabel;

    // PatternView::Listener implementation
    void patternLengthChanged(sirkus::ui::PatternView *view,
                              int newLength) override;
    void trackMidiChannelChanged(sirkus::ui::PatternView *view, int trackIndex,
                                 int newChannel) override;
    void stepStateChanged(sirkus::ui::PatternView *view, int trackIndex,
                          int stepIndex) override;
    void stepSelectionChanged(sirkus::ui::PatternView *view) override;

    // StepControls::Listener implementation
    void noteValueChanged(sirkus::ui::StepControls *controls,
                          int newValue) override;
    void velocityChanged(sirkus::ui::StepControls *controls,
                         int newValue) override;
    void noteLengthChanged(sirkus::ui::StepControls *controls,
                           sirkus::NoteLength newLength) override;

    // GlobalControls::Listener implementation
    void timeSignatureChanged(sirkus::ui::GlobalControls *controls,
                              int numerator, int denominator) override;
    void stepIntervalChanged(sirkus::ui::GlobalControls *controls,
                             sirkus::StepInterval newInterval) override;

    void updatePositionDisplay();
    void updateTransportDisplay();
    void updatePatternView();
    void updateSelectedSteps();
    void updatePlaybackPosition();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SirkusAudioProcessorEditor)
};
