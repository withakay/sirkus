#pragma once


#include "../JuceHeader.h"

namespace Sirkus::Core {

using namespace juce;

/*

ValueTreeObject is a base class for objects that are stored in a ValueTree. It provides
convenience methods for setting and getting properties in the ValueTree, and automatically
adds itself as a listener to the ValueTree.

Example usage of TypedProperty and ValueTreeObject

// In Identifiers.h
namespace IDs
{
    DECLARE_ID(foo)
    DECLARE_ID(midiChannel)
    DECLARE_ID(bpm)
}

class MyObject : public ValueTreeObject
{
public:
    MyObject(ValueTree parentState, UndoManager& undoManager)
        : ValueTreeObject(parentState, IDs::myObject, undoManager)
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
    static inline const TypedProperty<Type> name              \
    {                                                         \
        juce::Identifier(#name), defaultVal                   \
    }

class ValueTreeObject : public ValueTree::Listener
{
protected:
    ValueTreeObject(ValueTree parentState, const Identifier& type, UndoManager& undoManagerToUse, int index = -1)
            : state(type)
            , undoManager(undoManagerToUse)
    {
        parentState.addChild(state, index, &undoManager);
        state.addListener(this);
    }

    // New constructor for existing state
    ValueTreeObject(ValueTree existingState, UndoManager& undoManagerToUse)
            : state(existingState)
            , undoManager(undoManagerToUse)
    {
        // TODO: is this a duplicate listener? Does it matter?
        state.addListener(this);
    }

    // Copy constructor
    ValueTreeObject(const ValueTreeObject& other)
            : state(other.state)
            , undoManager(other.undoManager)
    {
        state.addListener(this);
    }

    // Copy assignment operator
    ValueTreeObject& operator=(const ValueTreeObject& other)
    {
        if (this != &other)
        {
            state.removeListener(this);
            state = other.state;
            state.addListener(this);
        }
        return *this;
    }

    ~ValueTreeObject() override
    {
        state.removeListener(this);
    }

public:
    template <typename T>
    void setProperty(const Identifier& id, T value)
    {
        state.setProperty(id, VariantConverter<T>::toVar(value), &undoManager);
    }

    template <typename T>
    void setProperty(const TypedProperty<T>& property, T value)
    {
        state.setProperty(property.id, VariantConverter<T>::toVar(value), &undoManager);
    }

    template <typename T>
    T getProperty(const Identifier& id, T defaultValue) const
    {
        auto var = state.getProperty(id, defaultValue);
        return VariantConverter<T>::fromVar(var);
    }

    template <typename T>
    T getProperty(const TypedProperty<T>& property) const
    {
        auto var = state.getProperty(property.id, VariantConverter<T>::toVar(property.defaultValue));
        return VariantConverter<T>::fromVar(var);
    }

protected:
    template <typename T>
    void initProperty(const TypedProperty<T>& property)
    {
        if (!state.hasProperty(property.id))
            setProperty(property, VariantConverter<T>::toVar(property.defaultValue));
    }

    ValueTree state;
    UndoManager& undoManager;
};

} // namespace Sirkus::Core
