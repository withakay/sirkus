#include "StepTrack.h"

namespace Sirkus::UI {

StepTrack::StepTrack()
{
    // Create MIDI channel selector
    midiChannelSelector = std::make_unique<juce::ComboBox>();
    midiChannelSelector->addItem("Omni", 1);
    for (int i = 1; i <= 16; ++i)
        midiChannelSelector->addItem(juce::String(i), i + 1);
    midiChannelSelector->setSelectedId(1, juce::dontSendNotification);
    midiChannelSelector->onChange = [this] {
        setMidiChannel(midiChannelSelector->getSelectedId() - 2);
    };
    addAndMakeVisible(midiChannelSelector.get());

    // Create track label
    trackLabel = std::make_unique<juce::Label>();
    trackLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(trackLabel.get());

    // Create step buttons
    for (int i = 0; i < VISIBLE_STEPS; ++i)
    {
        auto button = std::make_unique<StepButton>();
        button->addListener(this);
        addAndMakeVisible(button.get());
        stepButtons.push_back(std::move(button));
    }

    setOpaque(true);
}

StepTrack::~StepTrack()
{
    for (auto& button : stepButtons)
        button->removeListener(this);
}

void StepTrack::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    // Draw track header background
    g.setColour(juce::Colours::black);
    g.fillRect(getLocalBounds().removeFromLeft(120));
}

void StepTrack::resized()
{
    auto bounds = getLocalBounds();

    // Layout track header
    auto header = bounds.removeFromLeft(120);
    trackLabel->setBounds(header.removeFromTop(30));
    midiChannelSelector->setBounds(header.reduced(10, 5));

    // Layout step buttons
    const int buttonWidth = juce::roundToInt(40 * zoomFactor);
    const int buttonHeight = juce::roundToInt(30 * zoomFactor);
    const int buttonSpacing = juce::roundToInt(5 * zoomFactor);

    bounds.reduce(buttonSpacing, buttonSpacing);
    auto buttonBounds = bounds.withHeight(buttonHeight);

    for (auto& button : stepButtons)
    {
        button->setBounds(buttonBounds.withWidth(buttonWidth));
        buttonBounds.translate(buttonWidth + buttonSpacing, 0);
    }
}

void StepTrack::setMidiChannel(int channel)
{
    if (midiChannel != channel)
    {
        midiChannel = channel;
        listeners.call(
            [this](Listener& l) {
                l.trackMidiChannelChanged(this, midiChannel);
            });
    }
}

void StepTrack::setTrackNumber(int number)
{
    if (trackNumber != number)
    {
        trackNumber = number;
        trackLabel->setText(
            "Track " + juce::String(number + 1),
            juce::dontSendNotification);
    }
}

void StepTrack::setZoomFactor(float factor)
{
    static constexpr float epsilon = 0.0001f;
    if (std::abs(zoomFactor - factor) > epsilon)
    {
        zoomFactor = factor;
        for (auto& button : stepButtons)
            button->setZoomFactor(factor);
        resized();
    }
}

void StepTrack::setCurrentPage(int newPage)
{
    if (currentPage != newPage && newPage >= 0 && newPage < totalPages)
    {
        currentPage = newPage;
        // Update button states based on new page
        repaint();
    }
}

void StepTrack::setTotalPages(int numPages)
{
    if (totalPages != numPages && numPages > 0)
    {
        totalPages = numPages;
        currentPage = juce::jmin(currentPage, totalPages - 1);
        repaint();
    }
}

void StepTrack::clearStepSelection()
{
    selectedStepIndices.clear();
    lastSelectedStepIndex = -1;

    for (auto& button : stepButtons)
        button->setSelected(false);
}

void StepTrack::clearAllTriggers()
{
    for (auto& button : stepButtons)
        button->setTriggered(false);
}

void StepTrack::setStepTriggered(int stepIndex, bool triggered)
{
    // Convert global step index to visible step index
    int visibleIndex = stepIndex % VISIBLE_STEPS;
    if (visibleIndex >= 0 &&
        static_cast<size_t>(visibleIndex) < stepButtons.size())
    {
        stepButtons[static_cast<size_t>(visibleIndex)]->setTriggered(triggered);
    }
}

void StepTrack::stepButtonClicked(
    StepButton* button,
    const juce::ModifierKeys& mods)
{
    handleStepSelection(button, mods);

    // Notify state change
    if (auto index = getStepButtonIndex(button); index >= 0)
    {
        int globalIndex = currentPage * VISIBLE_STEPS + index;
        listeners.call(
            [this, globalIndex](Listener& l) {
                l.stepStateChanged(this, globalIndex);
            });
    }
}

void StepTrack::stepButtonStateChanged(StepButton* button)
{
    if (auto index = getStepButtonIndex(button); index >= 0)
    {
        int globalIndex = currentPage * VISIBLE_STEPS + index;
        listeners.call(
            [this, globalIndex](Listener& l) {
                l.stepStateChanged(this, globalIndex);
            });
    }
}

void StepTrack::stepButtonRangeSelect(StepButton* button)
{
    if (lastSelectedStepIndex >= 0)
    {
        if (auto currentIndex = getStepButtonIndex(button); currentIndex >= 0)
        {
            selectStepRange(lastSelectedStepIndex, currentIndex);
        }
    }
}

void StepTrack::handleStepSelection(
    StepButton* button,
    const juce::ModifierKeys& mods)
{
    auto index = getStepButtonIndex(button);
    if (index < 0)
        return;

    if (mods.isShiftDown() && lastSelectedStepIndex >= 0)
    {
        // Range selection
        selectStepRange(lastSelectedStepIndex, index);
    }
    else
    {
        if (!mods.isCommandDown() && !mods.isCtrlDown())
        {
            // Clear previous selection unless Cmd/Ctrl is held
            clearStepSelection();
        }

        // Toggle selection for this step
        auto globalIndex = currentPage * VISIBLE_STEPS + index;
        auto it = std::find(
            selectedStepIndices.begin(),
            selectedStepIndices.end(),
            globalIndex);

        if (it == selectedStepIndices.end())
        {
            selectedStepIndices.push_back(globalIndex);
            button->setSelected(true);
        }
        else if (mods.isCommandDown() || mods.isCtrlDown())
        {
            selectedStepIndices.erase(it);
            button->setSelected(false);
        }

        lastSelectedStepIndex = index;
    }
}

void StepTrack::selectStepRange(int fromIndex, int toIndex)
{
    // Ensure indices are in order
    if (fromIndex > toIndex)
        std::swap(fromIndex, toIndex);

    // Clear previous selection
    clearStepSelection();

    // Select all steps in range
    for (int i = fromIndex;
         i <= toIndex && i < static_cast<int>(stepButtons.size()); ++i)
    {
        if (i >= 0)
        {
            int globalIndex = currentPage * VISIBLE_STEPS + i;
            selectedStepIndices.push_back(globalIndex);
            stepButtons[static_cast<size_t>(i)]->setSelected(true);
        }
    }
}

int StepTrack::getStepButtonIndex(StepButton* button) const
{
    auto it =
        std::find_if(
            stepButtons.begin(),
            stepButtons.end(),
            [button](const auto& ptr) {
                return ptr.get() == button;
            });

    return it != stepButtons.end()
               ? static_cast<int>(std::distance(stepButtons.begin(), it))
               : -1;
}

void StepTrack::addListener(Listener* listener)
{
    listeners.add(listener);
}

void StepTrack::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

} // namespace Sirkus::UI
