#pragma once

#include "PatternTrack.h"
#include "TrackPanelConfig.h"

#include <JuceHeader.h>

namespace Sirkus::UI {

class TrackPanel : public juce::Component, private PatternTrack::Listener
{
public:
    /**
    * @brief Interface for listening to track panel events
    *
    * This class defines the interface for objects that want to receive
    * notifications about changes in the track panel.
    */
    class Listener
    {
    protected:
        Listener() = default; // Protected default constructor
    public:
        virtual ~Listener() noexcept = default;
        Listener(const Listener&) = delete;
        Listener& operator=(const Listener&) = delete;
        Listener(Listener&&) = delete;
        Listener& operator=(Listener&&) = delete;

        /**
      * @brief Called when a track's MIDI channel changes
      * @param panel Pointer to the TrackPanel that changed
      * @param trackIndex Index of the track that changed
      * @param newChannel The new MIDI channel
      */
        virtual void trackMidiChannelChanged(const TrackPanel* panel, int trackIndex, int newChannel) noexcept = 0;

        /**
      * @brief Called when a step's state changes
      * @param panel Pointer to the TrackPanel that changed
      * @param trackIndex Index of the track containing the step
      * @param stepIndex Index of the step that changed
      */
        virtual void stepStateChanged(const TrackPanel* panel, int trackIndex, int stepIndex) noexcept = 0;

        /**
      * @brief Called when step selection changes
      * @param panel Pointer to the TrackPanel where selection changed
      */
        virtual void stepSelectionChanged(const TrackPanel* panel) noexcept = 0;

        /**
      * @brief Called when a pattern's length changes
      * @param panel Pointer to the TrackPanel that changed
      * @param trackIndex Index of the track containing the pattern
      * @param newLength The new pattern length
      */
        virtual void patternLengthChanged(const TrackPanel* panel, int trackIndex, int newLength) noexcept = 0;

        /**
      * @brief Called when a track's page changes
      * @param panel Pointer to the TrackPanel that changed
      * @param trackIndex Index of the track that changed
      * @param newPage The new page number
      */
        virtual void pageChanged(const TrackPanel* panel, int trackIndex, int newPage) noexcept = 0;
    };

    TrackPanel();
    ~TrackPanel() override;

    // Delete move operations since JUCE Component doesn't support them
    TrackPanel(TrackPanel&&) = delete;
    TrackPanel& operator=(TrackPanel&&) = delete;

    void paint(juce::Graphics& /*g*/) override;
    void resized() override;

    // Selection methods
    void clearAllStepSelections();
    [[nodiscard]] const std::vector<int>& getSelectedStepsForTrack(int trackIndex) const;
    [[nodiscard]] std::vector<std::pair<int, int>> getAllSelectedSteps() const;
    [[nodiscard]] PatternTrack* getTrack(int trackIndex);

    // Trigger methods
    void clearAllTriggers() const;
    void setStepTriggered(int trackIndex, int stepIndex, bool triggered) const;

    void addListener(Listener* listener)
    {
        jassert(listener != nullptr);
        listeners.add(listener);
    }

    void removeListener(Listener* listener)
    {
        jassert(listener != nullptr);
        listeners.remove(listener);
    }

private:
    // PatternTrack::Listener implementation
    void trackMidiChannelChanged(PatternTrack* track, int newChannel) override;
    void stepStateChanged(PatternTrack* track, int stepIndex) override;
    void patternLengthChanged(PatternTrack* track, int newLength) override;
    void pageChanged(PatternTrack* track, int newPage) override;

    [[nodiscard]]
    int getTrackIndex(const PatternTrack* track) const;

    // Tracks
    std::vector<std::unique_ptr<PatternTrack>> tracks;

    juce::ListenerList<Listener> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackPanel)
};

} // namespace Sirkus::UI
