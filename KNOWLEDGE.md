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

### Key Components
1. **Transport System**
   - Internal transport management
   - Host synchronization
   - MIDI clock handling

2. **Timing System**
   - PPQ-based timing
   - Time division handling
   - Swing processing

3. **MIDI Event Generation**
   - Step-based event generation
   - Real-time MIDI output
   - Event processing pipeline

4. **Step Sequencing**
   - Pattern-based sequencing
   - Complex rhythmic features
   - Scale-aware note processing

## Data Structures

### Core Structures
1. **Track**
   - Time division configuration
   - Step storage
   - Modulation routing
   - Polyrhythmic properties

2. **Step Types**
   - Note steps with repeat capabilities
   - CC/NRPN steps
   - Auxiliary events

3. **Modulation System**
   - Modulation routes
   - Accumulator processing
   - State management

### State Management
- Sequence state
- Pattern chains
- Event processing context
- Real-time parameter updates

## Development Progress

### Completed Components
- Basic project structure with CMake build system
- JUCE plugin infrastructure (AudioProcessor, Editor)
- Timing management system with:
  - Host sync capabilities (TimingManager)
  - Internal transport with BPM and time signature control
  - Musical position tracking
  - Sample-accurate timing
- MIDI infrastructure:
  - Basic event generation framework (PPQN = 960)
  - Sample-accurate event timing

### In Progress
- Transport system refinement
- MIDI event generation expansion
- Step sequencing core implementation
- Pattern management system design

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
- High-resolution PPQN (960) for accurate timing
- Sample-accurate event generation
- Host-synchronized timing
- Buffer management with JUCE MidiBuffer
- Standalone and plugin mode support
- Extensible event generation pipeline

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