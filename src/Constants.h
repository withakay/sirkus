#pragma once

#ifndef SIRKUS_CONSTANTS_H
#define SIRKUS_CONSTANTS_H


namespace Sirkus::Constants {

// Timing constants
static constexpr int MAX_STEPS = 128; // Maximum steps per pattern
static constexpr int MAX_TRACKS = 16; // Maximum number of tracks
static constexpr int PPQN = 960;      // Pulses Per Quarter Note

// Base interval constants
static constexpr int STEP_128TH = PPQN / 32;     // 30 ticks
static constexpr int STEP_64TH = PPQN / 16;      // 60 ticks
static constexpr int STEP_32ND = PPQN / 8;       // 120 ticks
static constexpr int STEP_16TH = PPQN / 4;       // 240 ticks
static constexpr int STEP_8TH = PPQN / 2;        // 480 ticks
static constexpr int STEP_QUARTER = PPQN;        // 960 ticks
static constexpr int STEP_HALF = PPQN * 2;       // 1920 ticks
static constexpr int STEP_WHOLE = PPQN * 4;      // 3840 ticks
static constexpr int STEP_TWO_BARS = PPQN * 8;   // 7680 ticks
static constexpr int STEP_FOUR_BARS = PPQN * 16; // 15360 ticks

// Dotted interval constants (1.5x base value)
static constexpr int STEP_DOTTED_128TH = STEP_128TH * 3 / 2;    // 45 ticks
static constexpr int STEP_DOTTED_64TH = STEP_64TH * 3 / 2;      // 90 ticks
static constexpr int STEP_DOTTED_32ND = STEP_32ND * 3 / 2;      // 180 ticks
static constexpr int STEP_DOTTED_16TH = STEP_16TH * 3 / 2;      // 360 ticks
static constexpr int STEP_DOTTED_8TH = STEP_8TH * 3 / 2;        // 720 ticks
static constexpr int STEP_DOTTED_QUARTER = STEP_QUARTER * 3 / 2; // 1440 ticks
static constexpr int STEP_DOTTED_HALF = STEP_HALF * 3 / 2;      // 2880 ticks
static constexpr int STEP_DOTTED_WHOLE = STEP_WHOLE * 3 / 2;    // 5760 ticks

// Triplet interval constants (2/3 of the next larger base value)
static constexpr int STEP_TRIPLET_128TH = STEP_64TH * 2 / 3;    // 40 ticks
static constexpr int STEP_TRIPLET_64TH = STEP_32ND * 2 / 3;     // 80 ticks
static constexpr int STEP_TRIPLET_32ND = STEP_16TH * 2 / 3;     // 160 ticks
static constexpr int STEP_TRIPLET_16TH = STEP_8TH * 2 / 3;      // 320 ticks
static constexpr int STEP_TRIPLET_8TH = STEP_QUARTER * 2 / 3;   // 640 ticks
static constexpr int STEP_TRIPLET_QUARTER = STEP_HALF * 2 / 3;  // 1280 ticks
static constexpr int STEP_TRIPLET_HALF = STEP_WHOLE * 2 / 3;    // 2560 ticks
static constexpr int STEP_TRIPLET_WHOLE = STEP_TWO_BARS * 2 / 3;// 5120 ticks

// Note length constants (in ticks)
static constexpr int NOTE_128TH = PPQN / 32;        // 30 ticks (STEP_64TH / 2)
static constexpr int NOTE_64TH = PPQN / 16;         // 60 ticks
static constexpr int NOTE_32ND = PPQN / 8;          // 120 ticks
static constexpr int NOTE_16TH = PPQN / 4;          // 240 ticks
static constexpr int NOTE_8TH = PPQN / 2;           // 480 ticks
static constexpr int NOTE_QUARTER = PPQN;           // 960 ticks
static constexpr int NOTE_HALF = PPQN * 2;          // 1920 ticks
static constexpr int NOTE_WHOLE = PPQN * 4;         // 3840 ticks
static constexpr int NOTE_TWO_BARS = PPQN * 8;      // 7680 ticks
static constexpr int NOTE_FOUR_BARS = PPQN * 16;    // 15360 ticks
static constexpr int NOTE_EIGHT_BARS = PPQN * 32;   // 30720 ticks
static constexpr int NOTE_SIXTEEN_BARS = PPQN * 64; // 61440 ticks

// Macro for suppressing unused parameter warnings
#define SIRKUS_UNUSED(x) (void)x

} // namespace Sirkus::Core

#endif // SIRKUS_CONSTANTS_H
