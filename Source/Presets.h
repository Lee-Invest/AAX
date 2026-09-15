#pragma once
#include <juce_core/juce_core.h>
#include <vector>
#include <map>

struct PresetInfo
{
    juce::String name;
    std::map<juce::String, float> values;
};

// Original presets inspired by (not claiming to be official) classic Queen-era
// guitar tones. Approximate starting points, not certified recreations.
std::vector<PresetInfo> getFactoryPresets();
