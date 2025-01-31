#pragma once

#ifndef SIRKUS_CONSTANTS_H
#define SIRKUS_CONSTANTS_H


namespace Sirkus::Constants {

// Timing constants
static constexpr int PPQN = 960;      // Pulses Per Quarter Note
static constexpr int MAX_STEPS = 128; // Maximum steps per pattern
static constexpr int MAX_TRACKS = 16; // Maximum number of tracks

// Step interval constants (grid spacing, in ticks)
static constexpr int STEP_64TH = PPQN / 16;      // 60 ticks
static constexpr int STEP_32ND = PPQN / 8;       // 120 ticks
static constexpr int STEP_16TH = PPQN / 4;       // 240 ticks
static constexpr int STEP_8TH = PPQN / 2;        // 480 ticks
static constexpr int STEP_QUARTER = PPQN;        // 960 ticks
static constexpr int STEP_HALF = PPQN * 2;       // 1920 ticks
static constexpr int STEP_WHOLE = PPQN * 4;      // 3840 ticks
static constexpr int STEP_TWO_BARS = PPQN * 8;   // 7680 ticks
static constexpr int STEP_FOUR_BARS = PPQN * 16; // 15360 ticks

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
