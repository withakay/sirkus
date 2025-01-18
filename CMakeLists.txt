cmake_minimum_required(VERSION 3.30)

project(Sirkus VERSION 0.1.0)

# Always use the newest C++ standard on green-field projects if possible.
# Currently, JUCE supports only C++ 20 cross-platform.
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_XCODE_GENERATE_SCHEME OFF)
#set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# I like to download the dependencies to the same folder as the project.
# If you want to install them system wide, set CPM_SOURCE_CACHE with the path to the dependencies
# either as an environment variable or pass it to the cmake script with -DCPM_SOURCE_CACHE=<path>.
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# Downloads CPM if not already downloaded. CPM is an easy-to-use package manager nicely integrated with CMake.
include(cmake/cpm.cmake)

# This commands downloads AND configures JUCE. It sets up some variables, like JUCE_SOURCE_DIR.
CPMAddPackage(
    NAME JUCE
    GIT_TAG 8.0.4
    VERSION 8.0.4
    GITHUB_REPOSITORY juce-framework/JUCE
    SOURCE_DIR ${LIB_DIR}/juce
)


# Plugin definition using juce_add_plugin
juce_add_plugin(${PROJECT_NAME}
    VERSION 0.1.0                             # Plugin version number matches the .jucer file
    COMPANY_NAME "Fader"                      # Plugin manufacturer
    IS_SYNTH FALSE                            # Not defined as a synth in .juce r
    NEEDS_MIDI_INPUT TRUE                     # pluginWantsMidiIn in .jucer
    NEEDS_MIDI_OUTPUT TRUE                    # pluginProducesMidiOut in .jucer
    IS_MIDI_EFFECT TRUE                       # pluginIsMidiEffectPlugin in .jucer
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE          # Assuming the editor needs keyboard focus
    COPY_PLUGIN_AFTER_BUILD TRUE              # Standard for plugin builds
    PLUGIN_MANUFACTURER_CODE FADR             # Matches the .jucer file
    PLUGIN_CODE SRKS                          # Matches the .jucer file
    PLUGIN_VST3_CATEGORY Fx                   # Matches the .jucer file
    FORMATS AU VST3 Standalone                # Formats from .jucer file
    PRODUCT_NAME "Sirkus"                     # Simplified product name for binaries
)

juce_generate_juce_header(${PROJECT_NAME})

# This lets us use our code in both the JUCE targets and our Test target
# Without running into ODR violations
add_library(SharedCode INTERFACE)

# Make sure you include any new source files here
set(SourceFiles
    src/PluginProcessor.cpp
    src/PluginProcessor.h
    src/PluginEditor.cpp
    src/PluginEditor.h
)

# Make the SourceFiles buildable
target_sources(SharedCode INTERFACE ${SourceFiles})

include(cmake/xcode-prettify.cmake)

# Set JUCE compile definitions
target_compile_definitions(SharedCode
    INTERFACE
    
    JUCE_WEB_BROWSER=0                     # Disabling web browser unless needed
    JUCE_USE_CURL=0                        # Disabling curl unless needed
    JUCE_VST3_CAN_REPLACE_VST2=0          # Matches .jucer file JUCE_VST3_CAN_REPLACE_VST2

    # lets the app known if we're Debug or Release
    CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    VERSION="${CURRENT_VERSION}"

    # JucePlugin_Name is for some reason doesn't use the nicer PRODUCT_NAME
    PRODUCT_NAME_WITHOUT_VERSION="Sirkus"
)

# Link JUCE modules and recommended settings
target_link_libraries(SharedCode
    INTERFACE
    juce_audio_utils
    juce_gui_basics
    juce_gui_extra
    juce_core
    juce_data_structures
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# Link the JUCE plugin targets our SharedCode target
target_link_libraries("${PROJECT_NAME}" PRIVATE SharedCode)

