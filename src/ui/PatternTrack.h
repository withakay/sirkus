#pragma once

#include "../JuceHeader.h"
#include "StepComponent.h"

#include <vector>

#include "../core/Track.h"

namespace Sirkus::UI {

class PatternTrack : public juce::Component, private StepComponent::Listener
{
public:
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void trackMidiChannelChanged(PatternTrack* track, int newChannel) = 0;
        virtual void stepStateChanged(PatternTrack* track, int stepIndex) = 0;
        virtual void patternLengthChanged(PatternTrack* track, int newLength) = 0;
        virtual void pageChanged(PatternTrack* track, int newPage) = 0;
    };

    static constexpr int VISIBLE_STEPS = 16;
    static constexpr int MIN_PATTERN_LENGTH = 1;
    static constexpr int MAX_PATTERN_LENGTH = 128;

    //explicit PatternTrack(int trackNumber);
    explicit PatternTrack(Sirkus::Core::Track& track);
    ~PatternTrack() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    // Track settings
    void setMidiChannel(int channel);
    void setTrackNumber(int number);

    // Pattern controls
    void setPatternLength(int length);

    [[nodiscard]] int getPatternLength() const
    {
        return patternLength;
    }

    // Page management
    void setCurrentPage(int newPage);
    void setTotalPages(int numPages);

    [[nodiscard]] int getCurrentPage() const
    {
        return currentPage;
    }

    [[nodiscard]] int getTotalPages() const
    {
        return totalPages;
    }

    // Selection methods
    void clearStepSelection();

    [[nodiscard]] const std::vector<int>& getSelectedStepIndices() const
    {
        return selectedStepIndices;
    }

    [[nodiscard]] int getLastSelectedStepIndex() const
    {
        return lastSelectedStepIndex;
    }

    // Step state methods
    void clearAllTriggers();
    void setStepTriggered(int stepIndex, bool triggered);
    void setStepEnabled(int stepIndex, bool enabled);

    void addListener(Listener* listener);
    void removeListener(Listener* listener);

private:
    // StepComponent::Listener implementation
    void stepButtonClicked(StepButton* button, const juce::ModifierKeys& mods) override;
    void stepButtonStateChanged(StepButton* button) override;
    void stepButtonRangeSelect(StepButton* button) override;

    void handleStepSelection(StepButton* button, const juce::ModifierKeys& mods);
    void selectStepRange(int fromIndex, int toIndex);
    int getStepButtonIndex(StepButton* button) const;
    void updatePageControls();
    void updateStepButtonStates();

    // Track header controls
    std::unique_ptr<juce::ComboBox> midiChannelSelector;
    std::unique_ptr<juce::Label> trackLabel;

    // Pattern controls
    std::unique_ptr<juce::Slider> patternLengthSlider;
    std::unique_ptr<juce::Label> patternLengthLabel;
    std::unique_ptr<juce::TextButton> prevPageButton;
    std::unique_ptr<juce::TextButton> nextPageButton;
    std::unique_ptr<juce::Label> pageLabel;

    std::vector<std::unique_ptr<StepComponent>> stepComponents;
    std::vector<int> selectedStepIndices;

    Core::Track* track;
    int trackNumber;
    int midiChannel{-1}; // -1 = omni
    int currentPage{0};
    int totalPages{1};
    int patternLength{16};
    int lastSelectedStepIndex{-1};

    juce::ListenerList<Listener> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternTrack)
};

} // namespace Sirkus::UI
