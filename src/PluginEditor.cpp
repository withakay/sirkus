#include "PluginEditor.h"

#include "Constants.h"
#include "core/Sequencer.h"
#include "core/TimingManager.h"
#include "core/Types.h"
#include <cstddef>
#include <cstdint>

#include "JuceHeader.h"

SirkusAudioProcessorEditor::SirkusAudioProcessorEditor(SirkusAudioProcessor& p)
    : AudioProcessorEditor(&p)
      , processorRef(p)
      , transportControls(p)
{
    // Apply custom look and feel
    setLookAndFeel(&lookAndFeel);

    // Add and make visible all components
    addAndMakeVisible(transportControls);
    addAndMakeVisible(globalControls);
    addAndMakeVisible(trackPanel);
    addAndMakeVisible(stepControls);
    addAndMakeVisible(midiEventLog);

    // Add listeners
    trackPanel.addListener(this);
    stepControls.addListener(this);
    globalControls.addListener(this);

    // Labels setup
    addAndMakeVisible(positionLabel);
    positionLabel.setJustificationType(juce::Justification::left);
    positionLabel.setText("Position: --", juce::dontSendNotification);
    positionLabel.getProperties().set("isValue", true);

    addAndMakeVisible(bpmLabel);
    bpmLabel.setJustificationType(juce::Justification::left);
    bpmLabel.setText("BPM: --", juce::dontSendNotification);
    bpmLabel.getProperties().set("isValue", true);

    addAndMakeVisible(timeSignatureLabel);
    timeSignatureLabel.setJustificationType(juce::Justification::left);
    timeSignatureLabel.setText("Time Sig: --", juce::dontSendNotification);
    timeSignatureLabel.getProperties().set("isValue", true);

    // Initialize UI state from processor
    //updateTrackPanel();

    // Size setup
    setSize(1000, 800);

    // Start timer for updates
    startTimerHz(60); // 60 fps update rate
}

SirkusAudioProcessorEditor::~SirkusAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    stopTimer();
    trackPanel.removeListener(this);
    stepControls.removeListener(this);
    globalControls.removeListener(this);
}

void SirkusAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void SirkusAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10);

    // Top section: Transport and Global Controls
    auto topSection = area.removeFromTop(70);
    transportControls.setBounds(topSection.removeFromLeft(topSection.getWidth() / 2));
    globalControls.setBounds(topSection);

    area.removeFromTop(10); // spacing

    // Info labels section
    auto infoSection = area.removeFromTop(25);
    positionLabel.setBounds(infoSection.removeFromLeft(200));
    bpmLabel.setBounds(infoSection.removeFromLeft(100));
    timeSignatureLabel.setBounds(infoSection.removeFromLeft(100));

    area.removeFromTop(10); // spacing

    // Track Panel (main sequencer grid)
    const auto trackSection = area.removeFromTop(area.getHeight() - 200); // Reduced to make room for MIDI log
    trackPanel.setBounds(trackSection);

    area.removeFromTop(10); // spacing

    // MIDI Event Log
    const auto logSection = area.removeFromTop(100);
    midiEventLog.setBounds(logSection);

    area.removeFromTop(10); // spacing

    // Step Controls at bottom
    stepControls.setBounds(area);
}

void SirkusAudioProcessorEditor::timerCallback()
{
    updatePositionDisplay();
    updateTransportDisplay();
    updatePlaybackPosition();

    // Process any new MIDI messages
    auto messages = processorRef.getAndClearLatestMidiMessages();
    for (const auto metadata : messages)
    {
        midiEventLog.logMidiMessage(metadata.getMessage());
    }
}

void SirkusAudioProcessorEditor::updatePositionDisplay()
{
    auto& sequencer = processorRef.getSequencer();
    auto& timing = sequencer.getTimingManager();

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
    // Transport state is handled by TransportControls component
}

void SirkusAudioProcessorEditor::updatePlaybackPosition()
{
    auto& sequencer = processorRef.getSequencer();
    const auto& timing = sequencer.getTimingManager();

    // Clear all step triggers first
    trackPanel.clearAllTriggers();

    // Early return if not playing
    if (!timing.isPlaying())
        return;

    // Early return if no position available
    const auto pos = timing.getMusicalPosition();
    if (!pos)
        return;

    // Calculate total ticks once
    const int totalTicks = (((pos->bar - 1) * 4 + (pos->beat - 1)) * Sirkus::Core::PPQN) + static_cast<int>(pos->tick);

    // Process each track
    for (size_t trackIndex = 0; trackIndex < sequencer.getTrackCount(); ++trackIndex)
    {
        const auto& track = sequencer.getTrack(static_cast<uint32_t>(trackIndex));
        // if (track == nullptr)
        //     continue;

        const auto& pattern = track.getCurrentPattern();
        // if (pattern == nullptr)
        //     continue;

        // Calculate step interval ticks
        const int stepIntervalTicks = Sirkus::Core::stepIntervalToTicks(pattern.getStepInterval());
        if (stepIntervalTicks <= 0) // Safeguard against division by zero
            continue;

        // Calculate current step with pattern length wrapping
        const int patternLength = static_cast<int>(pattern.getLength());
        int currentStep = totalTicks / stepIntervalTicks;

        // Handle negative positions
        if (currentStep < 0)
        {
            currentStep = patternLength + (currentStep % patternLength);
        }

        // Wrap around pattern length
        currentStep %= patternLength;

        // Pass the global step index to the track panel
        trackPanel.setStepTriggered(static_cast<int>(trackIndex), currentStep, true);
    }
}

void SirkusAudioProcessorEditor::updateTrackPanel()
{
    auto& sequencer = processorRef.getSequencer();

    // Update step interval
    const auto& pattern = sequencer.getTrack(0).getCurrentPattern();
    globalControls.setStepInterval(pattern.getStepInterval());

    // Update MIDI channels for all tracks
    // for (size_t i = 0; i < sequencer.getTrackCount(); ++i)
    // {
    //     auto& track = sequencer.getTrack(static_cast<uint32_t>(i))
    //
    //         // Track IDs might not match index, so store mapping for future use
    //         SIRKUS_UNUSED(track->getId());
    //         SIRKUS_UNUSED(track->getMidiChannel());
    //         // TODO: Update track MIDI channel in UI once selection system is
    //         // implemented
    //
    // }
}

void SirkusAudioProcessorEditor::updateSelectedSteps()
{
    auto& sequencer = processorRef.getSequencer();
    const auto selectedSteps = trackPanel.getAllSelectedSteps();

    if (!selectedSteps.empty())
    {
        // Use the first selected step to initialize the controls
        auto [trackIndex, stepIndex] = selectedSteps[0];

        auto& pattern = sequencer.getCurrentPatternForTrack(static_cast<uint32_t>(trackIndex));

        const auto& step = pattern.getStep(static_cast<size_t>(stepIndex));
        stepControls.setNoteValue(step.getNote());
        stepControls.setVelocity(step.getVelocity());
        stepControls.setNoteLength(step.getNoteLength());

    }
}

void SirkusAudioProcessorEditor::trackMidiChannelChanged(
    const Sirkus::UI::TrackPanel* panel,
    int trackIndex,
    int newChannel) noexcept
{
    SIRKUS_UNUSED(panel); // Will be used for future panel-specific operations

    auto& sequencer = processorRef.getSequencer();
    auto& track = sequencer.getTrack(static_cast<uint32_t>(trackIndex));
    track.setMidiChannel(static_cast<uint8_t>(newChannel));
}

void SirkusAudioProcessorEditor::stepStateChanged(
    const Sirkus::UI::TrackPanel* panel,
    const int trackIndex,
    const int stepIndex) noexcept
{
    SIRKUS_UNUSED(panel); // Will be used for future panel-specific operations

    auto& sequencer = processorRef.getSequencer();
    auto& pattern = sequencer.getCurrentPatternForTrack(static_cast<uint32_t>(trackIndex));

    // Get the button's enabled state from the UI
    if (const auto& selectedSteps = trackPanel.getSelectedStepsForTrack(trackIndex);
        std::ranges::find(selectedSteps, stepIndex) != selectedSteps.end())
    {
        // Update only the enabled state, leave other properties unchanged
        pattern.setStepEnabled(static_cast<size_t>(stepIndex), true);
    }
    else
    {
        pattern.setStepEnabled(static_cast<size_t>(stepIndex), false);
    }
}

void SirkusAudioProcessorEditor::stepSelectionChanged(const Sirkus::UI::TrackPanel* panel) noexcept
{
    SIRKUS_UNUSED(panel); // Will be used for future panel-specific operations
    updateSelectedSteps();
}

void SirkusAudioProcessorEditor::pageChanged(
    const Sirkus::UI::TrackPanel* panel,
    const int trackIndex,
    const int newPage) noexcept
{
    SIRKUS_UNUSED(panel); // Will be used for future panel-specific operations

    auto& sequencer = processorRef.getSequencer();
    auto& pattern = sequencer.getCurrentPatternForTrack(static_cast<uint32_t>(trackIndex));

    // Calculate the range of steps visible on this page
    const int startStep = newPage * Sirkus::UI::PatternTrack::VISIBLE_STEPS;
    const int endStep = std::min(startStep + Sirkus::UI::PatternTrack::VISIBLE_STEPS, static_cast<int>(pattern.getLength()));

    // Get the track and update enabled state for each visible step
    if (auto* patternTrack = trackPanel.getTrack(trackIndex))
    {
        for (int i = startStep; i < endStep; ++i)
        {
            const auto& step = pattern.getStep(static_cast<size_t>(i));
            const int visibleIndex = i - startStep;
            patternTrack->setStepEnabled(visibleIndex, step.isEnabled());
        }
    }
}

void SirkusAudioProcessorEditor::patternLengthChanged(
    const Sirkus::UI::TrackPanel* panel,
    const int trackIndex,
    const int newLength) noexcept
{
    SIRKUS_UNUSED(panel); // Will be used for future panel-specific operations

    auto& sequencer = processorRef.getSequencer();
    auto& pattern = sequencer.getCurrentPatternForTrack(static_cast<uint32_t>(trackIndex));
    pattern.setLength(static_cast<size_t>(newLength));
}

// StepControls::Listener implementation
void SirkusAudioProcessorEditor::noteValueChanged(Sirkus::UI::StepControls* controls, const int newValue)
{
    SIRKUS_UNUSED(controls); // Will be used for control-specific operations

    auto& sequencer = processorRef.getSequencer();

    // Update note value for all selected steps
    for (const auto selectedSteps = trackPanel.getAllSelectedSteps();
         const auto& [trackIndex, stepIndex] : selectedSteps)
    {
        auto& pattern = sequencer.getCurrentPatternForTrack(static_cast<uint32_t>(trackIndex));
        pattern.setStepNote(static_cast<size_t>(stepIndex), static_cast<uint8_t>(newValue));
    }
}

void SirkusAudioProcessorEditor::velocityChanged(Sirkus::UI::StepControls* controls, const int newValue)
{
    SIRKUS_UNUSED(controls); // Will be used for control-specific operations

    auto& sequencer = processorRef.getSequencer();
    const auto selectedSteps = trackPanel.getAllSelectedSteps();

    // Update velocity for all selected steps
    for (const auto& [trackIndex, stepIndex] : selectedSteps)
    {
        auto& pattern = sequencer.getCurrentPatternForTrack(static_cast<uint32_t>(trackIndex));
        pattern.setStepVelocity(static_cast<size_t>(stepIndex), static_cast<uint8_t>(newValue));
    }
}

void SirkusAudioProcessorEditor::noteLengthChanged(
    Sirkus::UI::StepControls* controls,
    const Sirkus::Core::TimeDivision newLength)
{
    SIRKUS_UNUSED(controls); // Will be used for control-specific operations

    auto& sequencer = processorRef.getSequencer();
    const auto selectedSteps = trackPanel.getAllSelectedSteps();

    // Update note length for all selected steps
    for (const auto& [trackIndex, stepIndex] : selectedSteps)
    {
        auto& pattern = sequencer.getCurrentPatternForTrack(static_cast<uint32_t>(trackIndex));
        pattern.setStepNoteLength(static_cast<size_t>(stepIndex), newLength);
    }
}

// GlobalControls::Listener implementation
void SirkusAudioProcessorEditor::timeSignatureChanged(
    Sirkus::UI::GlobalControls* controls, const int numerator, const int denominator)
{
    SIRKUS_UNUSED(controls); // Will be used for control-specific operations

    auto& sequencer = processorRef.getSequencer();
    auto& timing = sequencer.getTimingManager();

    timing.setTimeSignature(numerator, denominator);
}

void SirkusAudioProcessorEditor::stepIntervalChanged(
    Sirkus::UI::GlobalControls* controls, const Sirkus::Core::TimeDivision newInterval)
{
    SIRKUS_UNUSED(controls); // Will be used for control-specific operations

    auto& sequencer = processorRef.getSequencer();

    // Update step interval for all tracks' patterns
    for (size_t i = 0; i < sequencer.getTrackCount(); ++i)
    {
        auto& pattern = sequencer.getCurrentPatternForTrack(static_cast<uint32_t>(i));
        pattern.setStepInterval(newInterval);
    }
}
