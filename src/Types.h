#pragma once

#include "Constants.h"

namespace sirkus {

// Scale quantization modes
enum class ScaleMode {
    Off,
    QuantizeUp,
    QuantizeDown,
    QuantizeRandom
};

// Step interval options (grid spacing)
enum class StepInterval {
    Sixty4th,    // 1/64 note
    Thirty2nd,   // 1/32 note
    Sixteen,     // 1/16 note
    Eighth,      // 1/8 note
    Quarter,     // 1/4 note
    Half,        // 1/2 note
    Whole,       // Whole note
    TwoBars,     // 2 bars
    FourBars     // 4 bars
};

// Note length options
enum class NoteLength {
    OneTwentyEighth,  // 1/128 note
    Sixty4th,         // 1/64 note
    Thirty2nd,        // 1/32 note
    Sixteen,          // 1/16 note
    Eighth,           // 1/8 note
    Quarter,          // 1/4 note
    Half,             // 1/2 note
    Whole,            // Whole note
    TwoBars,          // 2 bars
    FourBars,         // 4 bars
    EightBars,        // 8 bars
    SixteenBars       // 16 bars
};

// Helper function to convert StepInterval to ticks
inline int stepIntervalToTicks(StepInterval interval) {
    switch (interval) {
        case StepInterval::Sixty4th: return STEP_64TH;
        case StepInterval::Thirty2nd: return STEP_32ND;
        case StepInterval::Sixteen: return STEP_16TH;
        case StepInterval::Eighth: return STEP_8TH;
        case StepInterval::Quarter: return STEP_QUARTER;
        case StepInterval::Half: return STEP_HALF;
        case StepInterval::Whole: return STEP_WHOLE;
        case StepInterval::TwoBars: return STEP_TWO_BARS;
        case StepInterval::FourBars: return STEP_FOUR_BARS;
        default: return STEP_QUARTER;  // Default to quarter note
    }
}

// Helper function to convert NoteLength to ticks
inline int noteLengthToTicks(NoteLength length) {
    switch (length) {
        case NoteLength::OneTwentyEighth: return NOTE_128TH;
        case NoteLength::Sixty4th: return NOTE_64TH;
        case NoteLength::Thirty2nd: return NOTE_32ND;
        case NoteLength::Sixteen: return NOTE_16TH;
        case NoteLength::Eighth: return NOTE_8TH;
        case NoteLength::Quarter: return NOTE_QUARTER;
        case NoteLength::Half: return NOTE_HALF;
        case NoteLength::Whole: return NOTE_WHOLE;
        case NoteLength::TwoBars: return NOTE_TWO_BARS;
        case NoteLength::FourBars: return NOTE_FOUR_BARS;
        case NoteLength::EightBars: return NOTE_EIGHT_BARS;
        case NoteLength::SixteenBars: return NOTE_SIXTEEN_BARS;
        default: return NOTE_QUARTER;  // Default to quarter note
    }
}

} // namespace sirkus
