#pragma once

#include "PatternViewConfig.h"
#include "StepTrack.h"

#include <JuceHeader.h>

namespace Sirkus::UI {

class PatternView : public juce::Component, private StepTrack::Listener
{
public:
    /**
    * @brief Interface for listening to pattern view events
    *
    * This class defines the interface for objects that want to receive
    * notifications about changes in the pattern view.
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
      * @brief Called when the pattern length changes
      * @param view Pointer to the PatternView that changed
      * @param newLength The new length of the pattern
      */
        virtual void patternLengthChanged(const PatternView* view, int newLength) noexcept = 0;

        /**
      * @brief Called when a track's MIDI channel changes
      * @param view Pointer to the PatternView that changed
      * @param trackIndex Index of the track that changed
      * @param newChannel The new MIDI channel
      */
        virtual void trackMidiChannelChanged(const PatternView* view, int trackIndex, int newChannel) noexcept = 0;

        /**
      * @brief Called when a step's state changes
      * @param view Pointer to the PatternView that changed
      * @param trackIndex Index of the track containing the step
      * @param stepIndex Index of the step that changed
      */
        virtual void stepStateChanged(const PatternView* view, int trackIndex, int stepIndex) noexcept = 0;

        /**
      * @brief Called when step selection changes
      * @param view Pointer to the PatternView where selection changed
      */
        virtual void stepSelectionChanged(const PatternView* view) noexcept = 0;
    };

    PatternView();
    ~PatternView() override;
    PatternView(PatternView&&) noexcept;
    PatternView& operator=(PatternView&&) noexcept;

    void paint(juce::Graphics& /*g*/) override;
    void resized() override;

    /**
    * @brief Sets the pattern length
    * @param numSteps Must be between PatternViewConfig::minPatternLength and PatternViewConfig::maxPatternLength
    */
    void setPatternLength(int numSteps);

    /**
    * @brief Sets the current page being viewed
    * @param newPage Must be non-negative and less than total pages
    */
    void setCurrentPage(int newPage);

    /**
    * @brief Sets the zoom level for the view
    * @param level Must be between PatternViewConfig::minZoom and PatternViewConfig::maxZoom
    */
    void setZoomLevel(float level);

    // Selection methods
    void clearAllStepSelections();
    [[nodiscard]] const std::vector<int>& getSelectedStepsForTrack(int trackIndex) const;
    [[nodiscard]] std::vector<std::pair<int, int>> getAllSelectedSteps() const;

    // Trigger methods
    void clearAllTriggers();
    void setStepTriggered(int trackIndex, int stepIndex, bool triggered);

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
    // StepTrack::Listener implementation
    void trackMidiChannelChanged(StepTrack* track, int newChannel) override;
    void stepStateChanged(StepTrack* track, int stepIndex) override;

    void updatePageControls();
    void updatePatternLength();

    [[nodiscard]]
    int getNumPages() const;

    [[nodiscard]]
    int getTrackIndex(const StepTrack* track) const;

    // Pattern controls
    std::unique_ptr<juce::Label> patternLengthLabel;
    std::unique_ptr<juce::Slider> patternLengthSlider;

    // Page navigation
    std::unique_ptr<juce::TextButton> prevPageButton;
    std::unique_ptr<juce::TextButton> nextPageButton;
    std::unique_ptr<juce::Label> pageLabel;

    // Zoom control
    std::unique_ptr<juce::Slider> zoomSlider;

    // Pattern state
    int patternLength{PatternViewConfig::defaultPatternLength};
    int currentPage{PatternViewConfig::defaultPage};
    float zoomLevel{PatternViewConfig::defaultZoom};

    // Tracks
    std::vector<std::unique_ptr<StepTrack>> tracks;

    juce::ListenerList<Listener> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternView)
};

} // namespace Sirkus::UI
