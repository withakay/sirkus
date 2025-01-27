#pragma once

namespace Sirkus::UI::PatternViewConfig {

// Pattern configuration
constexpr int numTracks = 4;
constexpr int stepsPerPage = 16;
constexpr float minZoom = 0.5F;
constexpr float maxZoom = 2.0F;
constexpr int minPatternLength = 1;
constexpr int maxPatternLength = 64;
constexpr float zoomEpsilon = 0.0001F;

// UI Layout configuration
constexpr int topControlsHeight = 40;
constexpr int patternLengthLabelWidth = 100;
constexpr int patternLengthSliderWidth = 200;
constexpr int pageControlsWidth = 200;
constexpr int pageButtonWidth = 40;
constexpr int zoomSliderWidth = 150;
constexpr int trackSpacing = 10;

// Default values
constexpr int defaultPatternLength = 16;
constexpr float defaultZoom = 1.0F;
constexpr int defaultPage = 0;


} // namespace Sirkus::UI::PatternViewConfig
