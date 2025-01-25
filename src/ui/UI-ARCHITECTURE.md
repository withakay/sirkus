# UI Architecture Plan

## Component Hierarchy

### Core Components

1. `StepButton` (sirkus::ui::StepButton)
   - Properties:
     - on/off state
     - enabled state
     - trigger indicator
     - note value
     - velocity
     - note length
   - Appearance:
     - Visual feedback for different states
     - Size should scale with zoom level

2. `StepTrack` (sirkus::ui::StepTrack)
   - Contains:
     - 16 StepButtons (visible at once)
     - Track header with:
       - MIDI channel selector (0-15, 0 = omni)
       - Track number indicator
   - Behavior:
     - Handles paging for patterns > 16 steps
     - Updates step buttons based on current page

3. `PatternView` (sirkus::ui::PatternView)
   - Contains:
     - 4 StepTracks (initially, expandable design)
     - Pattern controls:
       - Pattern length control (1-128 steps)
       - Page navigation (when pattern > 16 steps)
       - Zoom controls
   - Behavior:
     - Manages track layout and spacing
     - Handles zoom level for all child components

4. `StepControls` (sirkus::ui::StepControls)
   - Properties:
     - Note value selector
     - Velocity control
     - Note length selector (using NoteLength enum)
   - Behavior:
     - Updates when step selection changes
     - Applies changes to selected step(s)

5. `GlobalControls` (sirkus::ui::GlobalControls)
   - Contains:
     - Time signature selector
     - Step interval selector (using StepInterval enum)
     - Pattern length control
     - Zoom controls

## Layout Strategy

### Main Layout
```
+------------------+
| Global Controls  |
+------------------+
| Pattern View     |
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

## Zoom Implementation

- Zoom levels: 0.5x, 1x, 1.5x, 2x
- Affects:
  - Step button size
  - Track spacing
  - Overall component scaling
- Maintains minimum usable sizes

## Component Communication

### Event System
- StepButton -> StepTrack: Step state changes
- StepTrack -> PatternView: Track state updates
- GlobalControls -> PatternView: Pattern length/zoom changes
- StepControls -> Selected steps: Parameter updates

### State Management
- Each track maintains its step data
- Pattern view coordinates track states
- Global settings stored at pattern level

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
   - Disabled state styling

## Implementation Plan

1. Phase 1: Core Components
   - Implement StepButton
   - Create StepTrack with basic functionality
   - Set up PatternView structure

2. Phase 2: Controls
   - Implement StepControls
   - Add GlobalControls
   - Connect control events to components

3. Phase 3: Advanced Features
   - Add zoom functionality
   - Implement paging system
   - Add MIDI channel selection

4. Phase 4: Polish
   - Refine styling
   - Add animations
   - Optimize performance

## Technical Considerations

1. Performance
   - Use JUCE's Component::setBufferedToImage() for complex components
   - Optimize repaints during pattern playback
   - Efficient handling of large patterns

2. Extensibility
   - Component design allows for future track count increases
   - Modular control system for easy feature additions
   - Flexible styling system for theme support

3. Resource Management
   - Smart pointer usage for component ownership
   - Efficient event handling
   - Proper cleanup in destructors