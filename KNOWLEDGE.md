# Sirkus Sequencer - Knowledge Base

## Project Overview
Sirkus is a JUCE-based step sequencer plugin and standalone app with sophisticated sequencing capabilities. The project follows an interface-based architecture emphasizing separation of concerns and testability.

## Core Architecture

### Design Principles
- Interface-based architecture
- Single responsibility per component
- Dependency injection
- Real-time processing considerations
- Testable and mockable components

### Coding Standards
- Naming Conventions (enforced by clang-tidy):
  - Classes: CamelCase (e.g., StepProcessor)
  - Methods: camelCase (e.g., processBlock)
  - Variables: camelCase (e.g., midiChannel)
  - Constants: UPPER_CASE (e.g., MAX_STEPS)
  - No underscores in names unless necessary for readability
  - Namespaces: lowercase (e.g., sirkus)

- Integer Type Usage:
  - Use size_t for:
    - Array indices (e.g., stepIndex)
    - Container sizes (e.g., pattern length)
    - Any index into std:: containers
    - Rationale: size_t is specifically for memory sizes/offsets per C spec
  - Use int for:
    - Tick values and positions
    - Musical timing calculations
    - Grid positions and intervals
    - Note lengths in ticks
    - Values that may be negative (offsets)
    - Rationale: Musical timing can be negative, ~2M ticks fit in int range
  - Use uint32_t for:
    - Track IDs and other identifiers
    - Rationale: IDs are always positive, uint32_t range is sufficient
  - Use uint8_t for:
    - MIDI values (notes: 0-127, velocity: 0-127)
    - Rationale: Naturally constrains to MIDI value range

### Key Components
1. **Sequencer**
   - Central orchestrator of the plugin
   - Manages tracks (1 to MAX_TRACKS)
   - Owns TimingManager and StepProcessor
   - Handles global parameters (swing, scale)
   - Provides track lifecycle management

2. **Scale System**
   - Comprehensive musical scale support
   - Built-in scales (Major, Minor, Modes, etc.)
   - Custom scale definition
   - Multiple quantization modes:
     - Up: Quantize to next scale note
     - Down: Quantize to previous scale note
     - Random: Randomly choose between up/down
   - Per-track scale settings
   - Global scale control

3. **Step System**
   - Grid-based step placement (64th to 4 bars)
   - Independent note lengths (128th to 16 bars)
   - Sample-accurate timing
   - Length-aware MIDI generation
   - Swing and offset support
   - Scale quantization integration
   - Pattern-based organization

4. **Transport System**
   - Internal transport management
   - Host synchronization
   - MIDI clock handling

## Data Structures

### Core Structures
1. **Step**
   - Note properties (note, velocity)
   - Timing properties:
     - Note length (128th to 16 bars)
     - Grid offset
     - Swing participation
   - Probability control
   - Track ownership reference
   - Length-aware timing calculations

2. **Pattern**
   - Step storage and management
   - Grid-based step placement:
     - Configurable step interval (64th to 4 bars)
     - Pattern length control
     - Swing settings
   - Step timing calculations:
     - Grid-based positioning
     - Swing application
     - Offset handling
     - Pattern wrapping

3. **StepProcessor**
   - MIDI event generation
   - Length-aware note timing
   - Note-off event management
   - Scale quantization
   - Future: Modulation processing

4. **Scale**
   - Built-in scale definitions
   - Custom scale support
   - Note quantization algorithms
   - Root note management

### Timing Constants
- PPQN (Pulses Per Quarter Note): 960

#### Step Intervals (Grid Spacing)
- 64th note: 60 ticks
- 32nd note: 120 ticks
- 16th note: 240 ticks
- 8th note: 480 ticks
- Quarter note: 960 ticks
- Half note: 1920 ticks
- Whole note: 3840 ticks
- Two bars: 7680 ticks
- Four bars: 15360 ticks

#### Note Lengths (Duration)
- 128th note: 30 ticks
- 64th note: 60 ticks
- 32nd note: 120 ticks
- 16th note: 240 ticks
- 8th note: 480 ticks
- Quarter note: 960 ticks
- Half note: 1920 ticks
- Whole note: 3840 ticks
- Two bars: 7680 ticks
- Four bars: 15360 ticks
- Eight bars: 30720 ticks
- Sixteen bars: 61440 ticks

### State Management
- Sequence state
- Pattern chains
- Event processing context
- Real-time parameter updates

### UI Architecture

1. **Component Hierarchy**
   - StepButton: Individual step UI with selection support
   - StepTrack: Track-level step management and selection
   - PatternView: Multi-track coordination and global selection
   - StepControls: Parameter editing for selected steps
   - GlobalControls: Pattern-wide settings

2. **Selection System**
   - Multi-mode selection:
     - Single click: Select individual step
     - Cmd/Ctrl+click: Add/remove from selection
     - Shift+click: Range selection within track
   - Selection state management:
     - Per-track selection tracking
     - Cross-track selection coordination
     - Parameter synchronization
   - Type-safe implementation:
     - Careful handling of signed/unsigned conversions
     - Explicit bounds checking
     - Safe MIDI value handling

3. **Event System**
   - Hierarchical event propagation:
     - StepButton -> StepTrack: Local state changes
     - StepTrack -> PatternView: Track-level updates
     - PatternView -> Editor: Global state changes
   - Parameter synchronization:
     - Selection-aware parameter updates
     - Batch updates for multiple steps
     - Real-time value propagation

4. **UI State Management**
   - Component state tracking:
     - Step enabled/disabled state
     - Selection state
     - Parameter values
   - Visual feedback:
     - Selection highlighting
     - Trigger indicators
     - Parameter value display
   - Page management:
     - 16-step visible window
     - Pattern length up to 128 steps
     - Page navigation controls

## Development Progress

### Completed Components
- Basic project structure with CMake build system
- JUCE plugin infrastructure
- Timing management system
- MIDI infrastructure
- Pattern/Track separation
- Scale System:
  - Comprehensive scale implementation
  - Multiple quantization modes
  - Global and per-track settings
- Step System:
  - Separate step interval (grid) and note length
  - Grid spacing from 64th to 4 bars
  - Note lengths from 128th to 16 bars
  - Length-aware MIDI generation
  - Sample-accurate timing
  - Pattern wrapping
  - Note-off event management
- Code Quality:
  - Consistent naming conventions
  - clang-tidy configuration
  - clang-format configuration
- Sequencer Implementation:
  - Central orchestration
  - Track management
  - Global parameter control
  - Integrated timing and MIDI processing
- UI System:
  - Component hierarchy with selection support
  - Multi-mode step selection:
    - Single click selection
    - Cmd/Ctrl+click for multi-select
    - Shift+click for range selection
  - Parameter editing for selected steps
  - Page navigation for patterns > 16 steps
  - Track-level MIDI channel control
  - Type-safe implementation:
    - Proper signed/unsigned handling
    - Array bounds checking
    - Safe MIDI value conversions
  - Real-time parameter updates
  - Visual feedback system

### In Progress
- Pattern management system:
  - Pattern switching mechanism
  - Pattern chaining
  - Pattern bank management
- Step processor expansion:
  - Modulation framework design
  - Cross-track parameter control
- UI Enhancements:
  - Keyboard shortcuts:
    - Cmd/Ctrl+A for select all
    - Delete/Backspace to clear steps
    - Copy/paste functionality
  - Selection improvements:
    - Drag selection support
    - Better visual feedback
    - Selection across pages
  - Visual refinements:
    - Improved step highlighting
    - Parameter value visualization
    - Animated transitions

### Planned Features
1. Core Engine (Epic 1)
   - Basic engine structure
   - Time management
   - Track system
   - Step sequencing
   - MIDI output
   - Transport controls
   - Pattern management

2. Event Processing (Epic 2)
   - Event pipeline
   - Aux event framework
   - Note processing
   - CC/NRPN handling
   - State management
   - Priority system

3. Time Division Features (Epic 3)
   - Track time divisions
   - Polyrhythm support
   - Swing processing
   - Step repeats
   - Micro-timing
   - Pattern modulation

## Technical Considerations

### Real-time Processing
- Lock-free data structures
- Audio thread safety
- Sample-accurate timing
- Efficient event processing
- Parameter smoothing
- State updates
- Performance monitoring

### MIDI System
- High-resolution PPQN (960)
- Sample-accurate event generation
- Length-aware note timing
- Host-synchronized timing
- Buffer management
- Extensible event pipeline

### Plugin Integration
- JUCE plugin wrapper
- Parameter automation
- Host synchronization
- Preset management
- MIDI learn system
- DAW state persistence

## Development Workflow
- Interface-first design approach
- Test-driven development encouraged
- Clear persona responsibilities
- Documented code conventions
- Performance-conscious implementation

This knowledge base will be updated as the project evolves and new insights are gained.