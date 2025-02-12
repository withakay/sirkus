#include "PatternTrack.h"

namespace Sirkus::UI {

PatternTrack::PatternTrack(int trackNum)
    : trackNumber(trackNum)
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
    trackLabel->setText("TR" + juce::String(trackNumber + 1), juce::dontSendNotification);
    addAndMakeVisible(trackLabel.get());

    // Create pattern length controls
    patternLengthLabel = std::make_unique<juce::Label>();
    patternLengthLabel->setText("Length", juce::dontSendNotification);
    patternLengthLabel->setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(patternLengthLabel.get());

    patternLengthSlider = std::make_unique<juce::Slider>(juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight);
    patternLengthSlider->setRange(MIN_PATTERN_LENGTH, MAX_PATTERN_LENGTH, 1);
    patternLengthSlider->setValue(patternLength, juce::dontSendNotification);
    patternLengthSlider->onValueChange = [this] {
        setPatternLength(static_cast<int>(patternLengthSlider->getValue()));
    };
    addAndMakeVisible(patternLengthSlider.get());

    // Create page navigation controls
    prevPageButton = std::make_unique<juce::TextButton>(juce::CharPointer_UTF8("\xe2\x97\x80")); // Left arrow
    prevPageButton->setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    prevPageButton->onClick = [this] {
        setCurrentPage(currentPage - 1);
    };
    addAndMakeVisible(prevPageButton.get());

    nextPageButton = std::make_unique<juce::TextButton>(juce::CharPointer_UTF8("\xe2\x96\xb6")); // Right arrow
    nextPageButton->setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    nextPageButton->onClick = [this] {
        setCurrentPage(currentPage + 1);
    };
    addAndMakeVisible(nextPageButton.get());

    pageLabel = std::make_unique<juce::Label>();
    pageLabel->setJustificationType(juce::Justification::centred);
    pageLabel->setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(pageLabel.get());

    // Create step components
    for (int i = 0; i < VISIBLE_STEPS; ++i)
    {
        auto component = std::make_unique<StepComponent>();
        component->addListener(this);
        addAndMakeVisible(component.get());
        stepComponents.push_back(std::move(component));
    }

    setOpaque(true);
    updatePageControls();
}

PatternTrack::~PatternTrack()
{
    for (auto& component : stepComponents)
        component->removeListener(this);
}

void PatternTrack::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    // Draw track header background
    g.setColour(juce::Colours::black);
    g.fillRect(getLocalBounds().removeFromLeft(250));
}

void PatternTrack::resized()
{
    auto bounds = getLocalBounds();

    // Layout track header (wider to accommodate controls)
    auto header = bounds.removeFromLeft(250);

    // Track info section
    auto trackInfo = header.removeFromTop(40);
    trackLabel->setBounds(trackInfo.removeFromTop(20));
    midiChannelSelector->setBounds(trackInfo.reduced(10, 0));

    // Pattern length section
    auto patternLengthSection = header.removeFromTop(40);
    patternLengthLabel->setBounds(patternLengthSection.removeFromLeft(50));
    patternLengthSlider->setBounds(patternLengthSection.reduced(5, 10));

    // Page navigation section
    auto pageSection = header.removeFromTop(40).reduced(5);
    prevPageButton->setBounds(pageSection.removeFromLeft(40));
    nextPageButton->setBounds(pageSection.removeFromRight(40));
    pageLabel->setBounds(pageSection);

    // Layout step components
    const int componentWidth = 40;
    const int componentHeight = 30;
    const int componentSpacing = 5;

    bounds.reduce(componentSpacing, componentSpacing);
    auto componentBounds = bounds.withHeight(componentHeight).withY(bounds.getCentreY() - componentHeight / 2);

    for (auto& component : stepComponents)
    {
        component->setBounds(componentBounds.withWidth(componentWidth));
        componentBounds.translate(componentWidth + componentSpacing, 0);
    }
}

void PatternTrack::setMidiChannel(int channel)
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

void PatternTrack::setTrackNumber(int number)
{
    trackNumber = number;
    trackLabel->setText("Track " + juce::String(number + 1), juce::dontSendNotification);
}

void PatternTrack::setPatternLength(int length)
{
    length = juce::jlimit(MIN_PATTERN_LENGTH, MAX_PATTERN_LENGTH, length);

    if (patternLength != length)
    {
        patternLength = length;
        patternLengthSlider->setValue(length, juce::dontSendNotification);
        setTotalPages((patternLength + VISIBLE_STEPS - 1) / VISIBLE_STEPS);

        listeners.call(
            [this](Listener& l) {
                l.patternLengthChanged(this, patternLength);
            });
    }
}

void PatternTrack::setCurrentPage(int newPage)
{
    if (currentPage != newPage && newPage >= 0 && newPage < totalPages)
    {
        currentPage = newPage;
        updatePageControls();
        updateStepButtonStates();
        listeners.call(
            [this](Listener& l) {
                l.pageChanged(this, currentPage);
            });
    }
}

void PatternTrack::setTotalPages(int numPages)
{
    if (totalPages != numPages && numPages > 0)
    {
        totalPages = numPages;
        currentPage = juce::jmin(currentPage, totalPages - 1);
        updatePageControls();
        updateStepButtonStates();
    }
}

void PatternTrack::updatePageControls()
{
    prevPageButton->setEnabled(currentPage > 0);
    nextPageButton->setEnabled(currentPage < totalPages - 1);
    pageLabel->setText(juce::String(currentPage + 1) + "/" + juce::String(totalPages), juce::dontSendNotification);
}

void PatternTrack::updateStepButtonStates()
{
    // Update selection state for visible components
    for (int i = 0; i < VISIBLE_STEPS; ++i)
    {
        int globalIndex = currentPage * VISIBLE_STEPS + i;
        auto it = std::find(selectedStepIndices.begin(), selectedStepIndices.end(), globalIndex);
        stepComponents[static_cast<size_t>(i)]->getStepButton()->setSelected(it != selectedStepIndices.end());
    }
}

void PatternTrack::clearStepSelection()
{
    selectedStepIndices.clear();
    lastSelectedStepIndex = -1;
    updateStepButtonStates();
}

void PatternTrack::clearAllTriggers()
{
    for (auto& component : stepComponents)
        component->getStepButton()->setTriggered(false);
}

void PatternTrack::setStepTriggered(int stepIndex, bool triggered)
{
    // Calculate which page this step belongs to
    const int stepPage = stepIndex / VISIBLE_STEPS;

    // Only show trigger if step is on current page
    if (stepPage == currentPage)
    {
        // Convert to visible step index
        if (const int visibleStepIndex = stepIndex % VISIBLE_STEPS;
            visibleStepIndex >= 0 && static_cast<size_t>(visibleStepIndex) < stepComponents.size())
        {
            stepComponents[static_cast<size_t>(visibleStepIndex)]->getStepButton()->setTriggered(triggered);
        }
    }
}

void PatternTrack::setStepEnabled(int stepIndex, bool enabled)
{
    if (stepIndex >= 0 && static_cast<size_t>(stepIndex) < stepComponents.size())
    {
        stepComponents[static_cast<size_t>(stepIndex)]->getStepButton()->setEnabled(enabled);
    }
}

void PatternTrack::stepButtonClicked(StepButton* button, const juce::ModifierKeys& mods)
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

void PatternTrack::stepButtonStateChanged(StepButton* button)
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

void PatternTrack::stepButtonRangeSelect(StepButton* button)
{
    if (lastSelectedStepIndex >= 0)
    {
        if (auto currentIndex = getStepButtonIndex(button); currentIndex >= 0)
        {
            selectStepRange(lastSelectedStepIndex, currentIndex);
        }
    }
}

void PatternTrack::handleStepSelection(StepButton* button, const juce::ModifierKeys& mods)
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
        auto it = std::find(selectedStepIndices.begin(), selectedStepIndices.end(), globalIndex);

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

void PatternTrack::selectStepRange(int fromIndex, int toIndex)
{
    // Ensure indices are in order
    if (fromIndex > toIndex)
        std::swap(fromIndex, toIndex);

    // Clear previous selection
    clearStepSelection();

    // Select all steps in range
    for (int i = fromIndex; i <= toIndex && i < static_cast<int>(stepComponents.size()); ++i)
    {
        if (i >= 0)
        {
            int globalIndex = currentPage * VISIBLE_STEPS + i;
            selectedStepIndices.push_back(globalIndex);
            stepComponents[static_cast<size_t>(i)]->getStepButton()->setSelected(true);
        }
    }
}

int PatternTrack::getStepButtonIndex(StepButton* button) const
{
    auto it = std::find_if(
        stepComponents.begin(),
        stepComponents.end(),
        [button](const auto& ptr) {
            return ptr->getStepButton() == button;
        });

    return it != stepComponents.end() ? static_cast<int>(std::distance(stepComponents.begin(), it)) : -1;
}

void PatternTrack::addListener(Listener* listener)
{
    listeners.add(listener);
}

void PatternTrack::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

} // namespace Sirkus::UI
