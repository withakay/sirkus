#pragma once

#include "Types.h"
#include <atomic>
#include <cstdint>

namespace sirkus {

struct Step {
    std::atomic<bool> enabled{false};
    std::atomic<uint8_t> note{60};
    std::atomic<uint8_t> velocity{100};
    std::atomic<float> probability{1.0f};
    std::atomic<float> timingOffset{0.0f};     // -1.0 to 1.0
    std::atomic<bool> affectedBySwing{true};
    std::atomic<uint64_t> triggerTick{0};
    std::atomic<uint32_t> trackId{0};          // Link back to owning track
    std::atomic<NoteLength> noteLength{NoteLength::Quarter};  // Duration of the note
    
    // Helper methods to get timing information
    uint64_t getNoteLengthInTicks() const {
        return noteLengthToTicks(noteLength.load(std::memory_order_acquire));
    }
};

} // namespace sirkus
