# UI Architecture Plan

## Component Hierarchy

### Core Components

1. `StepButton` (Sirkus::UI::StepButton)
  - Properties:
    - on/off state
    - enabled state
    - trigger indicator
    - selection state
    - note value
    - velocity
    - note length
  - Behavior:
    - Left click toggles enabled state
    - Cmd/Ctrl+click for multi-selection
    - Shift+click for range selection
    - Visual feedback for different states

2. `StepTrack` (Sirkus::UI::StepTrack)
  - Contains:
    - 16 StepButtons (visible at once)
    - Track header with:
      - MIDI channel selector (0-15, 0 = omni)
      - Track number indicator
    - Pattern controls:
      - Pattern length control (1-128 steps)
      - Page navigation (when pattern > 16 steps)
  - Behavior:
    - Handles paging for patterns > 16 steps
    - Manages step selection within track
    - Updates step buttons based on current page
    - Tracks last selected step for range selection

3. `TrackPanel` (Sirkus::UI::TrackPanel)
   - Contains:
     - 4 StepTracks (initially, expandable design)
   - Behavior:
     - Manages track layout and spacing
     - Coordinates selection across tracks
     - Provides selection state to parameter editors

4. `StepControls` (Sirkus::UI::StepControls)
  - Properties:
    - Note value selector (0-127)
    - Velocity control (0-127)
    - Note length selector (using TimeDivision enum)
  - Behavior:
    - Updates when step selection changes
    - Applies changes to all selected steps
    - Shows values from first selected step

5. `GlobalControls` (Sirkus::UI::GlobalControls)
  - Contains:
    - Time signature selector
    - Step interval selector (using StepInterval enum)
    - Pattern length control
    - Zoom controls

## Selection System

### Selection Modes

1. Single Selection
  - Left click selects single step
  - Clears previous selection

2. Multi Selection
  - Cmd/Ctrl+click adds/removes from selection
  - Works across tracks

3. Range Selection
  - Shift+click selects range from last selected
  - Works within single track

### Selection State Management

- Each track maintains its selected step indices
- TrackPanel coordinates selection across tracks
- Selection changes trigger parameter control updates
- Selected steps updated together when parameters change

## Layout Strategy

### Main Layout

```
+------------------+
| Global Controls  |
+------------------+
| Track Panel      |
|                 |
| [Track 1]       |
| [Track 2]       |
| [Track 3]       |
| [Track 4]       |
+------------------+
| Step Controls    |
+------------------+
```

### Track Layout

```
+----------------------------------------+
| Track Header | [][][][][]...[][] (16 steps) |
+----------------------------------------+
```

## Component Communication

### Event System

- StepButton -> StepTrack: Step state and selection changes
- StepTrack -> TrackPanel: Track state and selection updates
- GlobalControls -> TrackPanel: Pattern length/zoom changes
- StepControls -> Selected steps: Parameter updates

### State Management

- Each track maintains its step data
- Track panel coordinates track states
- Global settings stored at pattern level
- Selection state managed hierarchically

## Styling Guidelines

1. Consistent color scheme:
  - Step states (on/off)
  - Trigger indicators
  - Selection highlights
  - Track backgrounds

2. Grid alignment:
  - All components snap to grid
  - Consistent spacing and margins
  - Proper alignment between tracks

3. Interactive elements:
  - Clear hover states
  - Visual feedback for clicks
  - Selection highlighting
  - Disabled state styling

## Implementation Notes

1. Type Safety
  - Careful handling of signed/unsigned conversions
  - Explicit bounds checking for array access
  - Safe conversions for MIDI values

2. Performance
  - Use JUCE's Component::setBufferedToImage() for complex components
  - Optimize repaints during pattern playback
  - Efficient handling of large patterns

3. Extensibility
  - Component design allows for future track count increases
  - Modular control system for easy feature additions
  - Flexible styling system for theme support

4. Resource Management
  - Smart pointer usage for component ownership
  - Efficient event handling
  - Proper cleanup in destructors

## Future Enhancements

1. Keyboard Shortcuts
  - Cmd/Ctrl+A for select all
  - Delete/Backspace to clear steps
  - Copy/paste for selected steps

2. Visual Improvements
  - Drag selection
  - Better selection highlighting
  - Animated transitions

3. Additional Features
  - Step probability editing
  - Swing control per track
  - Pattern storage/recall
