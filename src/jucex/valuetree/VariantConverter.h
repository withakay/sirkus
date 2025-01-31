# pragma once

#ifndef SIRKUS_VARIANTCONVERTER_H
#define SIRKUS_VARIANTCONVERTER_H
#include "../../core/Types.h"

namespace juce {
//==============================================================================

#if JUCE_MODULE_AVAILABLE_juce_core

//==============================================================================
/** */
// template <>
// struct VariantConverter<uint8_t> final
// {
//     /** */
//     static uint8_t fromVar(const var& v)
//     {
//         if (v.isInt())
//             return static_cast<uint8_t>(static_cast<int>(v));
//
//         jassertfalse;
//         return 0;
//     }
//
//     /** */
//     static var toVar(const uint8_t& uint8)
//     {
//         return static_cast<int>(uint8);
//     }
// };

template <>
struct VariantConverter<unsigned char> final
{
    /** */
    static unsigned char fromVar(const var& v)
    {
        if (v.isInt())
            return static_cast<unsigned char>(static_cast<int>(v));

        jassertfalse;
        return 0;
    }

    /** */
    static var toVar(const unsigned char& uint8)
    {
        return static_cast<int>(uint8);
    }
};

template <>
struct VariantConverter<unsigned int> final
{
    /** */
    static unsigned int fromVar(const var& v)
    {
        if (v.isInt())
            return static_cast<unsigned int>(static_cast<int>(v));

        jassertfalse;
        return 0;
    }

    /** */
    static var toVar(const unsigned int& uint)
    {
        return static_cast<int>(uint);
    }
};

#define DECLARE_ENUM_VARIANT_CONVERTER(EnumType) \
template <> \
struct VariantConverter<EnumType> \
{ \
    static EnumType fromVar(const var& v) \
    { \
    return static_cast<EnumType>(static_cast<int>(v)); \
    } \
    \
    static var toVar(EnumType value) \
    { \
    return static_cast<int>(value); \
    } \
};

DECLARE_ENUM_VARIANT_CONVERTER(Sirkus::Core::NoteLength)

DECLARE_ENUM_VARIANT_CONVERTER(Sirkus::Core::StepInterval)

DECLARE_ENUM_VARIANT_CONVERTER(Sirkus::Core::ScaleMode)

#endif // JUCE_MODULE_AVAILABLE_juce_core

} // namespace juce

#endif // SIRKUS_VARIANTCONVERTER_H
