cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# make it easier to include cmake modules from ./cmake/
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Configure universal binaries which version of MacOS to support
# MUST be called before project()
include(macos-defaults)

# Always use the newest C++ standard on green-field projects if possible.
# Currently, JUCE supports only C++ 20 cross-platform.
include(juce-defaults)

# This is the internal name of the project and the name of JUCE's shared code target
# Note: This cannot have spaces (it may be 2024, but you can't have it all!)
# Worry not, JUCE's PRODUCT_NAME can have spaces (and is what DAWs display)
# set(PROJECT_NAME "Sirkus")
project(Sirkus VERSION 0.1.0)

# Worry not, JUCE's PRODUCT_NAME can have spaces (and is what DAWs will display)
# You can also just have it be the same thing as PROJECT_NAME
# You may want to append the major version on the end of this (and PROJECT_NAME) ala:
#   set(PROJECT_NAME "MyPlugin_v${MAJOR_VERSION}")
# Doing so enables major versions to show up in IDEs and DAWs as separate plugins
# allowing you to change parameters and behavior without breaking existing user projects
set(PRODUCT_NAME "Sirkus")

# Change me! Used for the MacOS bundle name and Installers
set(COMPANY_NAME "Fader")

# Change me! Used for the MacOS bundle identifier (and signing)
set(BUNDLE_ID "uk.fader.sirkus")

# Change me! Set the plugin formats you want built
# Valid choices: AAX Unity VST VST3 AU AUv3 Standalone
set(FORMATS Standalone AU VST3)


# I like to download the dependencies to the same folder as the project.
# If you want to install them system wide, set CPM_SOURCE_CACHE with the path to the dependencies
# either as an environment variable or pass it to the cmake script with -DCPM_SOURCE_CACHE=<path>.
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# Downloads CPM if not already downloaded. CPM is an easy-to-use package manager nicely integrated with CMake.
# include(cmake/cpm.cmake)

# This commands downloads AND configures JUCE. It sets up some variables, like JUCE_SOURCE_DIR.
# CPMAddPackage(
#         NAME JUCE
#         GIT_TAG 8.0.4
#         VERSION 8.0.4
#         GITHUB_REPOSITORY juce-framework/JUCE
#         SOURCE_DIR ${LIB_DIR}/juce
# )

add_subdirectory(${LIB_DIR}/juce)
add_subdirectory(${LIB_DIR}/melatonin_inspector)

# Plugin definition using juce_add_plugin
juce_add_plugin(${PROJECT_NAME}
        COMPANY_NAME "${COMPANY_NAME}"
        BUNDLE_ID "${BUNDLE_ID}"
        IS_SYNTH FALSE                            # Not defined as a synth in .juce r
        NEEDS_MIDI_INPUT TRUE                     # pluginWantsMidiIn in .jucer
        NEEDS_MIDI_OUTPUT TRUE                    # pluginProducesMidiOut in .jucer
        IS_MIDI_EFFECT TRUE                       # pluginIsMidiEffectPlugin in .jucer
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE          # Assuming the editor needs keyboard focus
        COPY_PLUGIN_AFTER_BUILD TRUE              # Standard for plugin builds
        PLUGIN_MANUFACTURER_CODE FADR             # Matches the .jucer file
        PLUGIN_CODE SRKS                          # Matches the .jucer file
        PLUGIN_VST3_CATEGORY Fx                   # Matches the .jucer file
        FORMATS "${FORMATS}"                      # Formats from .jucer file
        PRODUCT_NAME "${PRODUCT_NAME}"            # Simplified product name for binaries
)

target_link_libraries(${PROJECT_NAME} PRIVATE melatonin_inspector)
# This lets us use our code in both the JUCE targets and our Test target
# Without running into ODR violations
add_library(SharedCode INTERFACE)

# Make sure you include any new source files here
include(SourceFiles.cmake)

# Make the SourceFiles buildable
target_sources(SharedCode INTERFACE ${SourceFiles})

include(cmake/xcode-prettify.cmake)

# Set JUCE compile definitions
target_compile_definitions(SharedCode
        INTERFACE

        JUCE_WEB_BROWSER=0                     # Disabling web browser unless needed
        JUCE_USE_CURL=0                        # Disabling curl unless needed
        JUCE_VST3_CAN_REPLACE_VST2=0           # Matches .jucer file JUCE_VST3_CAN_REPLACE_VST2

        # lets the app known if we're Debug or Release
        CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
        VERSION="${CURRENT_VERSION}"

        # JucePlugin_Name is for some reason doesn't use the nicer PRODUCT_NAME
        PRODUCT_NAME_WITHOUT_VERSION="Sirkus"
)

juce_generate_juce_header(${PROJECT_NAME})

# Link JUCE modules and recommended settings
target_link_libraries(SharedCode
        INTERFACE

        juce_audio_basics
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

include_directories(${CMAKE_BINARY_DIR}/${PROJECT_NAME}_artefacts/JuceLibraryCode)

include(cmake/shared-code-defaults.cmake)
include(cmake/sanitizers.cmake)
