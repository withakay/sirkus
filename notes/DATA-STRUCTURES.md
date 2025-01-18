# Core Data Structures for Sequencer Engine

## Track Structure

````cpp
struct TimeDivision {
    uint32_t numerator;        // For representing ratios like 3:2
    uint32_t denominator;
    NoteValue baseValue;       // Base note value (whole, half, quarter etc.)
    bool triplet;              // Triplet modifier
    bool dotted;               // Dotted note modifier
};

enum class NoteValue {
    Bar4 = 16384,    // 4 bars
    Bar2 = 8192,     // 2 bars
    Bar = 4096,      // 1 bar
    Half = 2048,     // 1/2
    Quarter = 1024,  // 1/4
    Eighth = 512,    // 1/8
    Sixteenth = 256, // 1/16
    Thirty2nd = 128, // 1/32
    Sixty4th = 64,   // 1/64
    OneTwenty8th = 32 // 1/128
};

struct Track {
    string name;
    uint8_t channel;
    uint32_t length;           // Number of steps
    float startPosition;       // Modulatable start position
    float endPosition;         // Modulatable end position
    float currentPosition;     // Current playhead position
    vector<Step> steps;
    Scale scale;              // Current scale/key quantization
    vector<ModulationRoute> modulationRoutes;
    PlaybackMode playbackMode;  // Forward, Reverse, Pingpong, Random
    bool active;

    // Time division properties
    TimeDivision timeDivision;
    float swingAmount;         // Per-track swing
    float swingRate;          // Swing note value
    bool useGlobalSwing;      // Whether to use global or track swing

    // Polyrhythmic properties
    uint32_t cycleLength;     // Length in PPQ before pattern repeats
    bool maintainProportionalLength;  // Whether to maintain proportional length when changing time division
};

struct Scale {
    vector<uint8_t> notes;     // Scale notes (0-11)
    uint8_t root;              // Root note
    bool active;               // Whether quantization is enabled
};

## Step Event Types
```cpp
struct BaseStep {
    uint32_t position;
    bool active;
    float probability;        // Step trigger probability
    Condition condition;      // Step trigger condition
};

struct StepRepeat {
    RepeatMode mode;        // Type of repeat (ratchet, roll, etc.)
    uint8_t count;         // Number of repeats
    float probability;     // Probability per repeat
    float speedMultiplier; // Speed variation
    vector<uint8_t> velocityPattern;  // Velocity for each repeat
    vector<float> timingPattern;      // Timing offsets for repeats
    float decayAmount;    // Velocity decay per repeat
    float rampAmount;     // Velocity ramp up/down
    bool reversePattern;  // Reverse the repeat pattern
};

struct NoteStep : BaseStep {
    uint8_t note;            // MIDI note number
    uint8_t velocity;        // Note velocity
    float duration;          // Note duration in steps
    float microTiming;       // Micro timing adjustment
    StepRepeat repeat;      // Repeat settings

    // Advanced repeat options
    vector<uint8_t> notePattern;     // Different notes for each repeat
    float pitchBendAmount;           // Pitch bend per repeat
    uint8_t spreadAmount;            // Stereo spread for repeats
    ArpMode arpMode;                // Arpeggiator mode for repeats
};

enum class RepeatMode {
    Ratchet,        // Even subdivisions
    Roll,           // Roll with accent on first note
    Stutter,        // Rapid repeats with timing variations
    Echo,           // Repeats with decay
    Bounce,         // Alternating forward/reverse pattern
    Random,         // Random timing within step
    Euclidean,      // Euclidean rhythm distribution
    Polyrhythm,     // Custom polyrhythmic division
    Accelerating,   // Speed increases per repeat
    Decelerating    // Speed decreases per repeat
};

enum class ArpMode {
    Up,
    Down,
    UpDown,
    Random,
    AsPlayed,
    Chord          // Play all notes simultaneously
};

struct CCStep : BaseStep {
    uint8_t ccNumber;        // CC number
    float value;             // CC value (normalized 0-1)
    uint16_t nrpn;          // NRPN number (if used)
    bool isNRPN;            // Whether this is CC or NRPN
};

struct AuxEventState {
    uint32_t eventId;          // Reference to the event this state belongs to
    float currentValue;        // Current processed value
    float previousValue;       // Previous processed value
    vector<float> valueBuffer; // History buffer for delay lines etc
    uint32_t bufferPosition;   // Current position in buffer
    uint32_t stateFlags;       // Bit flags for various states
    uint32_t counter;          // General purpose counter
    time_t lastUpdateTime;     // Timestamp of last update

    // State machine specific
    uint32_t currentState;
    uint32_t previousState;
    vector<uint32_t> stateHistory;

    // Additional processing state
    float smoothValue;         // Current smoothed value
    bool triggerState;         // Current trigger state
    float accumulator;         // Running accumulator
    vector<bool> logicStates;  // States for logic operations
};

struct AuxEvent : BaseStep {
    AuxEventType type;       // Type of auxiliary event
    ModSource source;        // Modulation source
    ModTarget target;        // Modulation target
    float value;            // Initial/static value
    AccumulatorSettings accumulator;
    uint32_t stateBufferSize;  // Size of state history buffer
    bool maintainStateAcrossReset; // Whether to clear state on reset
    ProcessingMode processingMode;
};

struct ProcessingContext {
    double sampleRate;
    double tempo;
    double ppqPosition;
    bool isPlaying;
    vector<float> accumulatorValues;
    vector<bool> triggerStates;
    vector<float> modulationValues;
    map<uint32_t, AuxEventState> auxEventStates;  // Map of event ID to state
    double timeInSeconds;      // Current time for time-based processing
    uint32_t globalStepCount;  // Global step counter
    bool resetRequested;       // Whether a reset has been requested
};

// Helper struct for complex delay lines
struct DelayLineState {
    vector<float> buffer;
    uint32_t writePosition;
    uint32_t readPosition;
    float feedback;
    float wetDry;
};
````

## Modulation Structures

```cpp
struct ModulationRoute {
    uint32_t sourceTrackId;
    uint32_t sourceStepId;
    uint32_t targetTrackId;
    uint32_t targetParameterId;
    float amount;
    ProcessingMode processingMode;
};

struct AccumulatorSettings {
    AccumulatorMode mode;     // Linear, Exponential, etc.
    ClipMode clipMode;        // Clip, Wrap, Invert, Reset
    float min;                // Minimum value
    float max;                // Maximum value
    float rate;              // Accumulation rate
    TriggerCondition resetTrigger;
};

struct ModSource {
    SourceType type;         // Step value, Accumulator, External, etc.
    uint32_t id;            // Source identifier
    ProcessingMode processing; // How the source should be processed
};

struct ModTarget {
    TargetType type;         // Parameter, Step, Track property, etc.
    uint32_t id;            // Target identifier
    float scalingFactor;    // Value scaling
};
```

## Enumerations

```cpp
enum class AuxEventType {
    Accumulator,
    Math,
    Logic,
    Probability,
    StateMachine,
    DelayLine,
    SampleAndHold,
    Threshold,
    SlowLimiter
};

enum class AccumulatorMode {
    Linear,
    Exponential,
    Logarithmic
};

enum class ClipMode {
    Clip,
    Wrap,
    Invert,
    Reset
};

enum class ProcessingMode {
    Direct,
    Smoothed,
    Quantized,
    Randomized
};

enum class Condition {
    Always,
    Probability,
    EveryN,
    AfterN,
    OnAccumulator,
    OnLogic
};
```

## Sequence Management

````cpp
struct Sequence {
    string name;
    string author;
    string version;
    uint32_t id;                     // Unique sequence identifier
    vector<Pattern> patterns;
    vector<Track> globalTracks;      // Tracks that span across patterns
    SequenceSettings settings;
    TimeSignature timeSignature;
    double tempo;

    // Playback state
    uint32_t currentPatternIndex;
    bool isPlaying;
    double currentPosition;          // Global sequence position

    // Pattern chain configuration
    vector<PatternChain> patternChains;
    uint32_t activeChainIndex;

    // Global modulation routing
    vector<ModulationRoute> globalModulations;

    // Metadata for saving/loading
    time_t created;
    time_t lastModified;
    string tags;
    map<string, string> customMetadata;
};

struct SequenceSettings {
    bool syncToHost;
    bool sendMidiClock;
    bool receiveMidiClock;
    ClockSource clockSource;
    float globalSwing;
    float globalGroove;
    MidiPortConfig midiConfig;
};

struct TimeSignature {
    uint8_t numerator;
    uint8_t denominator;
};

struct PatternChain {
    string name;
    vector<uint32_t> patternOrder;   // Pattern indices in chain
    vector<ChainRule> transitionRules;
    bool loop;
    uint32_t loopCount;             // Number of times to loop (0 = infinite)
};

struct MidiPortConfig {
    vector<uint8_t> inputPorts;
    vector<uint8_t> outputPorts;
    map<uint8_t, uint8_t> portMapping;  // Input to output routing
    bool thruEnabled;
    vector<uint8_t> clockOutputPorts;
};

enum class ClockSource {
    Internal,
    MidiClock,
    HostSync,
    External
};

## Pattern Management
```cpp
struct Pattern {
    string name;
    vector<Track> tracks;
    uint32_t length;          // Global pattern length
    float globalSwing;        // Global swing amount
    vector<ChainRule> chainRules;  // Pattern chaining rules
};

struct ChainRule {
    uint32_t nextPatternId;
    Condition condition;
    uint32_t repeatCount;
};
````

## Event Processing Pipeline Structures

```cpp
struct ProcessingContext {
    double sampleRate;
    double tempo;
    double ppqPosition;
    bool isPlaying;
    vector<float> accumulatorValues;
    vector<bool> triggerStates;
    vector<float> modulationValues;
};

struct EventProcessor {
    virtual void process(ProcessingContext& context) = 0;
    virtual void reset() = 0;
};
```
