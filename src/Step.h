#pragma once

#include "Types.h"
#include <atomic>
#include <cstdint>

namespace Sirkus {

struct Step
{
    bool enabled{false};
    uint8_t note{60};
    uint8_t velocity{100};
    float probability{1.0f};
    float timingOffset{0.0f}; // -1.0 to 1.0
    bool affectedBySwing{true};
    int triggerTick{0};                         // Timing value, can be negative with offsets
    uint32_t trackId{0};                        // Link back to owning track
    NoteLength noteLength{NoteLength::Quarter}; // Duration of the note

    // Helper methods to get timing information
    int getNoteLengthInTicks() const
    {
        // Returns tick duration
        return noteLengthToTicks(noteLength);
    }
} __attribute__((aligned(32)));

} // namespace Sirkus
