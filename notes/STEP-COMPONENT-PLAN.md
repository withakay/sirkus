# StepComponent Implementation Plan

## Overview
Create a new UI component that will be used to wrap controls for each step. Initially, this will be an empty component that we can build upon later.

## Files to Create

### src/ui/StepComponent.h
```cpp
#pragma once

#include "../JuceHeader.h"

namespace Sirkus::UI {

class StepComponent : public juce::Component {
public:
    StepComponent();
    ~StepComponent() override = default;

    void paint(juce::Graphics& /* g */) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepComponent)
};

} // namespace Sirkus::UI
```

### src/ui/StepComponent.cpp
```cpp
#include "StepComponent.h"

namespace Sirkus::UI {

StepComponent::StepComponent()
{
    // Constructor implementation
}

void StepComponent::paint(juce::Graphics& g)
{
    // Paint implementation
}

void StepComponent::resized()
{
    // Resized implementation
}

} // namespace Sirkus::UI
```

## Next Steps
1. Review this initial implementation
2. Once approved, switch to Code mode to implement the component
3. Future iterations can add:
   - Specific controls and UI elements
   - Event handling
   - State management
   - Integration with the step logic