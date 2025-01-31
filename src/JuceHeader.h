/*
 * Extracted from JuceHeader.h to allow inserting of custom headers
 */

#pragma once

#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_events/juce_events.h>
#include <juce_core/juce_core.h>
// =======================================================
// Custom VariantConverter - needs to after juce_core
// but before any other module that uses it
#include "jucex/valuetree/VariantConverter.h"
// =======================================================
#include <juce_data_structures/juce_data_structures.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_devices/juce_audio_devices.h>


#if JUCE_TARGET_HAS_BINARY_DATA
 #include "BinaryData.h"
#endif


#if ! JUCE_DONT_DECLARE_PROJECTINFO
namespace ProjectInfo
{
const char* const projectName = "Sirkus";
const char* const companyName = "Fader";
const char* const versionString = "0.1.0";
const int versionNumber = 0x100;
}
#endif
