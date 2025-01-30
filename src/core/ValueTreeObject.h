#pragma once

#include <JuceHeader.h>

namespace Sirkus::Core {

/*

ValueTreeWrapper is a base class for objects that are stored in a ValueTree. It provides
convenience methods for setting and getting properties in the ValueTree, and automatically
adds itself as a listener to the ValueTree.

Example usage of TypedProperty and ValueTreeWrapper

// In Identifiers.h
namespace IDs
{
    DECLARE_ID(foo)
    DECLARE_ID(midiChannel)
    DECLARE_ID(bpm)
}

class MyObject : public ValueTreeWrapper
{
public:
    MyObject(ValueTree parentState, UndoManager& undoManager)
        : ValueTreeWrapper(parentState, IDs::myObject, undoManager)
    {
        // Set default values
        setProperty(TypedProperty<bool>{ IDs::foo }, false);
        setProperty(TypedProperty<int>{ IDs::midiChannel }, 1);
        setProperty(TypedProperty<float>{ IDs::bpm }, 120.0f);
    }

    // Accessors
    bool getFoo() const { return getProperty(TypedProperty<bool>{ IDs::foo }); }
    void setFoo(bool value) { setProperty(TypedProperty<bool>{ IDs::foo }, value); }

    int getMidiChannel() const { return getProperty(TypedProperty<int>{ IDs::midiChannel }); }
    void setMidiChannel(int value) { setProperty(TypedProperty<int>{ IDs::midiChannel }, value); }

    float getBpm() const { return getProperty(TypedProperty<float>{ IDs::bpm }); }
    void setBpm(float value) { setProperty(TypedProperty<float>{ IDs::bpm }, value); }
};

*/

template <typename T>
struct TypedProperty
{
    const juce::Identifier id;
    const T defaultValue;

    TypedProperty(const juce::Identifier& identifier, T defaultVal)
        : id(identifier)
          , defaultValue(defaultVal)
    {
    }
};

#define SIRKUS_DECLARE_TYPED_PROPERTY(Type, name, defaultVal) \
    static inline const TypedProperty<Type> name { juce::Identifier(#name), defaultVal }

class ValueTreeWrapper : public ValueTree::Listener
{
protected:
    ValueTreeWrapper(ValueTree parentState, const Identifier& type, UndoManager& undoManagerToUse)
        : state(type)
          , undoManager(undoManagerToUse)
    {
        parentState.addChild(state, -1, &undoManager);
        state.addListener(this);
    }

    ~ValueTreeWrapper() override
    {
        state.removeListener(this);
    }

    template <typename T>
    void setProperty(const Identifier& id, T value)
    {
        state.setProperty(id, value, &undoManager);
    }

    template <typename T>
    void setProperty(const TypedProperty<T>& property, T value)
    {
        state.setProperty(property.id, value, &undoManager);
    }

    template <typename T>
    T getProperty(const Identifier& id, T defaultValue) const
    {
        if (!state.hasProperty(id))
            return defaultValue;

        auto var = state.getProperty(id);
        return static_cast<T>(var);
    }

    template <typename T>
    T getProperty(const TypedProperty<T>& property) const
    {
        if (!state.hasProperty(property.id))
            return property.defaultValue;

        auto var = state.getProperty(property.id);
        return static_cast<T>(var);
    }

    template <typename T>
    void initProperty(const TypedProperty<T>& property)
    {
        if (!state.hasProperty(property.id))
            setProperty(property, property.defaultValue);
    }

protected:
    ValueTree state;
    UndoManager& undoManager;
};

} // namespace Sirkus::Core
