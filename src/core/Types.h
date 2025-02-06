#pragma once

#include "../Constants.h"
#include "Scale.h"
#include <cstdint>

namespace Sirkus::Core {

using namespace Sirkus::Constants;

// Scale quantization modes
enum class ScaleMode {
    Off,
    QuantizeUp,
    QuantizeDown,
    QuantizeRandom
};

// Track information needed for step processing
struct TrackInfo {
  uint32_t id;
  uint8_t midiChannel;
  ScaleMode scaleMode;
} __attribute__((aligned(16)));

enum TimeDivision {
    HundredTwentyEighthNote = STEP_128TH,
    DottedHundredTwentyEighthNote = STEP_DOTTED_128TH,
    TripletHundredTwentyEighthNote = STEP_TRIPLET_128TH,

    SixtyFourthNote = STEP_64TH,
    DottedSixtyFourthNote = STEP_DOTTED_64TH,
    TripletSixtyFourthNote = STEP_TRIPLET_64TH,

    ThirtySecondNote = STEP_32ND,
    DottedThirtySecondNote = STEP_DOTTED_32ND,
    TripletThirtySecondNote = STEP_TRIPLET_32ND,

    SixteenthNote = STEP_16TH,
    DottedSixteenthNote = STEP_DOTTED_16TH,
    TripletSixteenthNote = STEP_TRIPLET_16TH,

    EighthNote = STEP_8TH,
    DottedEighthNote = STEP_DOTTED_8TH,
    TripletEighthNote = STEP_TRIPLET_8TH,

    QuarterNote = STEP_QUARTER,
    DottedQuarterNote = STEP_DOTTED_QUARTER,
    TripletQuarterNote = STEP_TRIPLET_QUARTER,

    HalfNote = STEP_HALF,
    DottedHalfNote = STEP_DOTTED_HALF,
    TripletHalfNote = STEP_TRIPLET_HALF,

    WholeNote = STEP_WHOLE,
    DottedWholeNote = STEP_DOTTED_WHOLE,
    TripletWholeNote = STEP_TRIPLET_WHOLE,

    TwoBars = STEP_TWO_BARS,
    FourBars = STEP_FOUR_BARS
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
inline int stepIntervalToTicks(TimeDivision interval) {
    return interval;
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

} // namespace Sirkus::Core
