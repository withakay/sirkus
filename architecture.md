# Sirkus Architecture

```mermaid
graph TD
    %% Core Audio Processing
    subgraph Audio Engine
        AP[AudioProcessor] --> SEQ[Sequencer]
        SEQ --> TM[TimingManager]
        SEQ --> SP[StepProcessor]
        SEQ --> TR[Tracks]
        TR --> ST[Steps]
        TM --> TI[TimingInfo]
    end

    %% UI Components
    subgraph UI Layer
        PE[PluginEditor] --> GC[GlobalControls]
        PE --> PV[PatternView]
        PE --> SC[StepControls]
        PV --> STR[StepTracks]
        STR --> SB[StepButtons]
        PE --> TC[TransportControls]
    end

    %% Data Flow
    subgraph Data Flow
        PAT[Pattern] --> TR
        SCA[Scale] --> SEQ
        SB --> ST
        TC --> TM
    end

    %% Component Communication
    GC -->|Pattern Settings| PV
    SC -->|Step Parameters| SB
    SB -->|Step State| STR
    STR -->|Track State| PV
    PV -->|Pattern State| PE
    PE -->|UI State| AP
    AP -->|Audio/MIDI| OUT[DAW/Output]

    %% Styling
    classDef core fill:#f9f,stroke:#333,stroke-width:2px
    classDef ui fill:#bbf,stroke:#333,stroke-width:2px
    classDef data fill:#bfb,stroke:#333,stroke-width:2px

    class AP,SEQ,TM,SP core
    class PE,GC,PV,SC,STR,SB,TC ui
    class PAT,SCA,TR,ST data
```

## Component Descriptions

### Audio Engine
- **AudioProcessor**: Main JUCE plugin processor handling audio/MIDI I/O
- **Sequencer**: Core sequencing engine managing tracks and timing
- **TimingManager**: Handles synchronization with host or internal clock
- **StepProcessor**: Processes step data into MIDI events
- **Tracks**: Collection of sequencer tracks
- **Steps**: Individual step data within tracks

### UI Layer
- **PluginEditor**: Main plugin UI container
- **GlobalControls**: Pattern-wide settings (time signature, length)
- **PatternView**: Main pattern editing interface
- **StepControls**: Step parameter editing interface
- **StepTracks**: Track containers with step buttons
- **StepButtons**: Individual step UI elements
- **TransportControls**: Playback control interface

### Data Flow
- **Pattern**: Pattern data structure
- **Scale**: Musical scale information
- **Data Flow**: Shows how data moves between components

## Key Features
- MIDI sequencing with multiple tracks
- Host sync or standalone timing
- Scale-based note input
- Pattern-based sequencing
- Modular UI architecture
- Extensible track system