cmake_minimum_required(VERSION 3.22)

project(BrightonRig VERSION 1.0.0)

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
# Requires Avid's AAX SDK (free Avid Developer account, https://developer.avid.com)
# and PACE signing before Pro Tools will load an AAX build. Not needed for the
# VST3/AU path used with wrappers like MetaPlugin.
if(NOT AAX_SDK_PATH AND DEFINED ENV{AAX_SDK_PATH})
    set(AAX_SDK_PATH $ENV{AAX_SDK_PATH})
endif()

set(BRIGHTONRIG_FORMATS VST3 Standalone)
if(APPLE)
    list(APPEND BRIGHTONRIG_FORMATS AU)
endif()
if(AAX_SDK_PATH)
    juce_set_aax_sdk_path(${AAX_SDK_PATH})
    list(APPEND BRIGHTONRIG_FORMATS AAX)
endif()

juce_add_plugin(BrightonRig
    COMPANY_NAME "YourCompany"
    PLUGIN_MANUFACTURER_CODE Ycmp
    PLUGIN_CODE Brtg
    FORMATS ${BRIGHTONRIG_FORMATS}
    PRODUCT_NAME "Brighton Rig"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORIES "Fx" "Distortion"
    AAX_CATEGORY "AAX_ePlugInCategory_Modulation"
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(BrightonRig PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/DSP/NoiseGate.cpp
    Source/DSP/TrebleBooster.cpp
    Source/DSP/AmpStage.cpp
    Source/DSP/CabinetMic.cpp
    Source/DSP/ModDelayVerb.cpp
    Source/Presets.cpp
)

target_include_directories(BrightonRig PRIVATE Source)

target_compile_definitions(BrightonRig PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(BrightonRig PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_audio_processors
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
