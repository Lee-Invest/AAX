cmake_minimum_required(VERSION 3.22)

project(SimpleVerb VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.8
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

# --- AAX support (optional) -------------------------------------------------
# AAX requires Avid's AAX SDK, which is only available through a free Avid
# Developer account (https://developer.avid.com). It cannot be redistributed
# here. Once you have it, point AAX_SDK_PATH at the SDK root (the folder that
# contains "Interfaces") either via -DAAX_SDK_PATH=... on the cmake command
# line, or by setting the environment variable below, and re-run cmake.
# AAX plugins also require signing with a PACE Anti-Piracy developer account
# (wraptool) before Pro Tools will load them.
if(NOT AAX_SDK_PATH AND DEFINED ENV{AAX_SDK_PATH})
    set(AAX_SDK_PATH $ENV{AAX_SDK_PATH})
endif()

set(SIMPLEVERB_FORMATS VST3 Standalone)
if(APPLE)
    list(APPEND SIMPLEVERB_FORMATS AU)
endif()
if(AAX_SDK_PATH)
    juce_set_aax_sdk_path(${AAX_SDK_PATH})
    list(APPEND SIMPLEVERB_FORMATS AAX)
    message(STATUS "AAX SDK found at ${AAX_SDK_PATH} - AAX target enabled")
else()
    message(STATUS "AAX_SDK_PATH not set - building VST3/AU/Standalone only. "
                    "See README.md for how to enable AAX once you have the SDK.")
endif()

juce_add_plugin(SimpleVerb
    COMPANY_NAME "YourCompany"
    PLUGIN_MANUFACTURER_CODE Ycmp
    PLUGIN_CODE Svrb
    FORMATS ${SIMPLEVERB_FORMATS}
    PRODUCT_NAME "SimpleVerb"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORIES "Fx" "Reverb"
    AAX_CATEGORY "AAX_ePlugInCategory_Reverb"
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(SimpleVerb PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_compile_definitions(SimpleVerb PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(SimpleVerb PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_audio_processors
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
