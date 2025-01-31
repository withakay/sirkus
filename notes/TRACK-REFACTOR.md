# Track Refactoring Plan: ValueTreeWrapper Integration

## Overview
This document outlines the plan to refactor the Track class to use ValueTreeWrapper, similar to how Sequencer uses it. This change will improve state management, enable undo/redo functionality, and create a more consistent architecture across the codebase.

## Current Implementation
Track currently uses:
- Raw member variables for state (trackId, currentPattern)
- Atomic variables for thread-safe properties (midiChannel, scaleMode)
- Direct pattern ownership through std::unique_ptr

## ValueTree Structure

### Existing Identifiers
```cpp
// Base type identifiers
DECLARE_ID(track)
DECLARE_ID(pattern)
DECLARE_ID(step)

// Track properties
namespace Track {
    DECLARE_ID(trackId)
    DECLARE_ID(midiChannel)
    DECLARE_ID(scaleMode)
}
```

### Required New Identifiers
```cpp
// Pattern properties
namespace Pattern {
    DECLARE_ID(length)
    DECLARE_ID(swingAmount)
    DECLARE_ID(stepInterval)
}

// Step properties
namespace Step {
    DECLARE_ID(enabled)
    DECLARE_ID(note)
    DECLARE_ID(velocity)
    DECLARE_ID(probability)
    DECLARE_ID(timingOffset)
    DECLARE_ID(affectedBySwing)
    DECLARE_ID(triggerTick)
    DECLARE_ID(trackId)
    DECLARE_ID(noteLength)
}
```

### Class Structure
```cpp
class Track final : public ValueTreeWrapper
{
public:
    Track(ValueTree parentState, UndoManager& undoManagerToUse, uint32_t id);
    
    struct props
    {
        // Using existing identifiers
        static inline const TypedProperty<uint32_t> trackId{ID::Track::trackId, 0};
        static inline const TypedProperty<uint8_t> midiChannel{ID::Track::midiChannel, 1};
        static inline const TypedProperty<ScaleMode> scaleMode{ID::Track::scaleMode, ScaleMode::Off};
    };
};

class Pattern final : public ValueTreeWrapper
{
public:
    Pattern(ValueTree parentState, UndoManager& undoManagerToUse);
    
    struct props
    {
        static inline const TypedProperty<int> length{ID::Pattern::length, 16};
        static inline const TypedProperty<float> swingAmount{ID::Pattern::swingAmount, 0.0f};
        static inline const TypedProperty<StepInterval> stepInterval{ID::Pattern::stepInterval, StepInterval::Quarter};
    };
};

class Step final : public ValueTreeWrapper
{
public:
    Step(ValueTree parentState, UndoManager& undoManagerToUse);
    
    struct props
    {
        static inline const TypedProperty<bool> enabled{ID::Step::enabled, false};
        static inline const TypedProperty<uint8_t> note{ID::Step::note, 60};
        static inline const TypedProperty<uint8_t> velocity{ID::Step::velocity, 100};
        static inline const TypedProperty<float> probability{ID::Step::probability, 1.0f};
        static inline const TypedProperty<float> timingOffset{ID::Step::timingOffset, 0.0f};
        static inline const TypedProperty<bool> affectedBySwing{ID::Step::affectedBySwing, true};
        static inline const TypedProperty<int> triggerTick{ID::Step::triggerTick, 0};
        static inline const TypedProperty<uint32_t> trackId{ID::Step::trackId, 0};
        static inline const TypedProperty<NoteLength> noteLength{ID::Step::noteLength, NoteLength::Quarter};
    };
};
```

## ValueTree Hierarchy
```
Track (ID::track)
└── Pattern (ID::pattern)
    └── Steps (Array of ID::step)
```

## Implementation Changes

### 1. Track State Management
- Move state to ValueTree properties using existing identifiers:
  - trackId → props::trackId
  - midiChannel → props::midiChannel
  - scaleMode → props::scaleMode
- Pattern becomes a child ValueTree using ID::pattern identifier
- Properties accessible through ValueTreeWrapper methods

### 2. Pattern Integration
- Pattern class inherits from ValueTreeWrapper
- Properties moved to ValueTree:
  - length → props::length
  - swingAmount → props::swingAmount
  - stepInterval → props::stepInterval
- Track manages Pattern as child ValueTree using ID::pattern
- Steps managed as child ValueTrees using ID::step

### 3. Step Integration
- Step class inherits from ValueTreeWrapper
- All Step properties moved to ValueTree
- Pattern manages Steps as child ValueTrees
- Consider array-like access pattern for steps

### 4. Thread Safety
- Ensure ValueTree access is thread-safe
- Maintain atomic-like behavior where needed
- Consider using ValueTree::Listener for property updates

### 5. Required Changes in Sequencer
- Update Track creation to include ValueTree hierarchy
- Pass UndoManager to new Track instances
- Manage Track ValueTrees as children using ID::track identifier

## Implementation Steps

1. Add new Pattern and Step identifiers to Identifiers.h
2. Update Track constructor to use ValueTreeWrapper
3. Implement Track property getters/setters using existing identifiers
4. Create Pattern ValueTreeWrapper implementation
5. Create Step ValueTreeWrapper implementation
6. Implement Pattern property getters/setters
7. Implement Step property getters/setters
8. Modify Sequencer's track management
9. Update UI components to work with new structure
10. Add tests for new functionality

## Migration Strategy

1. Create new Track implementation
2. Update Sequencer to work with new Track
3. Update UI components
4. Add tests for new functionality
5. Remove old implementation

## Benefits

1. Consistent state management across codebase
2. Built-in undo/redo support
3. Improved state persistence
4. Better state synchronization
5. More maintainable codebase

## Risks and Mitigations

1. Thread Safety
   - Careful consideration of audio thread access
   - Proper use of atomic operations where needed

2. Migration Complexity
   - Incremental changes
   - Comprehensive testing
   - Clear documentation