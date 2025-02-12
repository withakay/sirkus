#include "TrackPanel.h"

#include "../Constants.h"

namespace Sirkus::UI {

TrackPanel::TrackPanel()
{
    // Create tracks
    for (int i = 0; i < TrackPanelConfig::numTracks; ++i)
    {
        auto track = std::make_unique<PatternTrack>(i);
        track->addListener(this);
        track->setTrackNumber(i);
        addAndMakeVisible(track.get());
        tracks.push_back(std::move(track));
    }

    setOpaque(true);
}

TrackPanel::~TrackPanel()
{
    for (const auto& track : tracks)
        track->removeListener(this);
}

void TrackPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void TrackPanel::resized()
{
    auto bounds = getLocalBounds();

    // Layout tracks with equal spacing
    bounds.removeFromTop(TrackPanelConfig::trackSpacing); // Top spacing
    const int trackHeight =
        (bounds.getHeight() - ((TrackPanelConfig::numTracks - 1) * TrackPanelConfig::trackSpacing)) /
        TrackPanelConfig::numTracks;

    for (const auto& track : tracks)
    {
        track->setBounds(bounds.removeFromTop(trackHeight));
        bounds.removeFromTop(TrackPanelConfig::trackSpacing); // Spacing between tracks
    }
}

void TrackPanel::clearAllStepSelections()
{
    for (const auto& track : tracks)
        track->clearStepSelection();

    listeners.call(
        [this](Listener& l) {
            l.stepSelectionChanged(this);
        });
}

void TrackPanel::clearAllTriggers() const
{
    for (auto& track : tracks)
        track->clearAllTriggers();
}

PatternTrack* TrackPanel::getTrack(int trackIndex)
{
    if (trackIndex >= 0 && static_cast<size_t>(trackIndex) < tracks.size())
        return tracks[static_cast<size_t>(trackIndex)].get();
    return nullptr;
}

void TrackPanel::setStepTriggered(const int trackIndex, const int stepIndex, const bool triggered) const
{
    if (trackIndex >= 0 && static_cast<size_t>(trackIndex) < tracks.size())
    {
        // DBG(
        //     "TrackPanel::setStepTriggered(track: " << trackIndex << ", step: " << stepIndex << ", triggered: " << std::
        //     to_string(triggered)
        //     << ")");
        tracks[static_cast<size_t>(trackIndex)]->setStepTriggered(stepIndex, triggered);
    }
}

const std::vector<int>& TrackPanel::getSelectedStepsForTrack(int trackIndex) const
{
    static constexpr std::vector<int> emptySelection;

    if (trackIndex >= 0 && static_cast<size_t>(trackIndex) < tracks.size())
        return tracks[static_cast<size_t>(trackIndex)]->getSelectedStepIndices();

    return emptySelection;
}

std::vector<std::pair<int, int>> TrackPanel::getAllSelectedSteps() const
{
    std::vector<std::pair<int, int>> allSelected;

    for (size_t trackIndex = 0; trackIndex < tracks.size(); ++trackIndex)
    {
        const auto& trackSelection = tracks[trackIndex]->getSelectedStepIndices();
        for (int stepIndex : trackSelection)
        {
            allSelected.emplace_back(static_cast<int>(trackIndex), stepIndex);
        }
    }

    return allSelected;
}

void TrackPanel::trackMidiChannelChanged(PatternTrack* track, int newChannel)
{
    if (auto trackIndex = getTrackIndex(track); trackIndex >= 0)
    {
        listeners.call(
            [this, trackIndex, newChannel](Listener& l) {
                l.trackMidiChannelChanged(this, trackIndex, newChannel);
            });
    }
}

void TrackPanel::stepStateChanged(PatternTrack* track, int stepIndex)
{
    if (auto trackIndex = getTrackIndex(track); trackIndex >= 0)
    {
        listeners.call(
            [this, trackIndex, stepIndex](Listener& l) {
                l.stepStateChanged(this, trackIndex, stepIndex);
            });

        // Selection may have changed
        listeners.call(
            [this](Listener& l) {
                l.stepSelectionChanged(this);
            });
    }
}

void TrackPanel::patternLengthChanged(PatternTrack* track, int newLength)
{
    if (auto trackIndex = getTrackIndex(track); trackIndex >= 0)
    {
        listeners.call(
            [this, trackIndex, newLength](Listener& l) {
                l.patternLengthChanged(this, trackIndex, newLength);
            });
    }
}

void TrackPanel::pageChanged(PatternTrack* track, int newPage)
{
    if (auto trackIndex = getTrackIndex(track); trackIndex >= 0)
    {
        listeners.call(
            [this, trackIndex, newPage](Listener& l) {
                l.pageChanged(this, trackIndex, newPage);
            });
    }
}

int TrackPanel::getTrackIndex(const PatternTrack* track) const
{
    auto it = std::find_if(
        tracks.begin(),
        tracks.end(),
        [track](const auto& ptr) {
            return ptr.get() == track;
        });

    return it != tracks.end() ? static_cast<int>(std::distance(tracks.begin(), it)) : -1;
}

} // namespace Sirkus::UI
