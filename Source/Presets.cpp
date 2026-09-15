#include "Presets.h"
#include "PluginProcessor.h"

using namespace ParamIDs;

std::vector<PresetInfo> getFactoryPresets()
{
    std::vector<PresetInfo> presets;

    auto add = [&](juce::String name, std::map<juce::String, float> v)
    {
        presets.push_back({ name, v });
    };

    add("Brighton Rock", {
        { boosterOn, 1.0f }, { boosterBoost, 0.85f }, { ampGain, 0.7f }, { ampTreble, 0.75f },
        { brightonRockMode, 1.0f }, { delayOn, 1.0f }, { brightonFeedback, 0.4f }, { brightonLevel, 0.5f },
        { reverbOn, 1.0f }, { reverbWet, 0.3f }
    });

    add("Bohemian-style Lead", {
        { boosterOn, 1.0f }, { boosterBoost, 0.75f }, { ampGain, 0.65f }, { ampSag, 0.5f },
        { ampPresence, 0.6f }, { delayOn, 1.0f }, { delayLevel, 0.25f }, { reverbWet, 0.3f }
    });

    add("Killer Queen", {
        { boosterOn, 1.0f }, { boosterBoost, 0.6f }, { ampGain, 0.5f }, { ampTreble, 0.7f },
        { chorusOn, 1.0f }, { chorusMix, 0.3f }, { reverbWet, 0.2f }
    });

    add("Keep Yourself Alive", {
        { boosterOn, 1.0f }, { boosterBoost, 0.8f }, { ampGain, 0.7f }, { ampMid, 0.65f },
        { delayOn, 1.0f }, { delayLevel, 0.2f }
    });

    add("Now I'm Here", {
        { boosterOn, 1.0f }, { boosterBoost, 0.8f }, { ampGain, 0.75f }, { ampPresence, 0.6f },
        { brightonRockMode, 1.0f }, { brightonLevel, 0.4f }
    });

    add("Tie Your Mother Down", {
        { boosterOn, 1.0f }, { boosterBoost, 0.9f }, { ampGain, 0.85f }, { ampSag, 0.55f },
        { ampMaster, 0.85f }
    });

    add("We Will Rock You", {
        { boosterOn, 0.0f }, { ampGain, 0.3f }, { ampBass, 0.7f }, { deacyBlend, 0.6f },
        { deacyDrive, 0.4f }, { reverbWet, 0.35f }
    });

    add("We Are The Champions", {
        { boosterOn, 1.0f }, { boosterBoost, 0.5f }, { ampGain, 0.4f }, { ampTreble, 0.55f },
        { reverbOn, 1.0f }, { reverbWet, 0.35f }, { reverbSize, 0.6f }
    });

    add("Somebody To Love", {
        { boosterOn, 1.0f }, { boosterBoost, 0.55f }, { ampGain, 0.45f }, { chorusOn, 1.0f },
        { chorusMix, 0.25f }, { reverbWet, 0.3f }
    });

    add("Don't Stop Me Now", {
        { boosterOn, 1.0f }, { boosterBoost, 0.7f }, { ampGain, 0.6f }, { ampTreble, 0.7f },
        { delayOn, 1.0f }, { delayLevel, 0.2f }
    });

    add("Fat Bottomed Girls", {
        { boosterOn, 1.0f }, { boosterBoost, 0.75f }, { ampGain, 0.65f }, { ampBass, 0.6f },
        { ampMid, 0.6f }
    });

    add("Hammer To Fall", {
        { boosterOn, 1.0f }, { boosterBoost, 0.85f }, { ampGain, 0.8f }, { ampSag, 0.5f },
        { ampMaster, 0.8f }
    });

    add("One Vision", {
        { boosterOn, 1.0f }, { boosterBoost, 0.7f }, { ampGain, 0.65f }, { phaserOn, 1.0f },
        { phaserDepth, 0.4f }, { reverbWet, 0.25f }
    });

    add("A Kind of Magic", {
        { boosterOn, 1.0f }, { boosterBoost, 0.65f }, { ampGain, 0.55f }, { chorusOn, 1.0f },
        { chorusMix, 0.3f }, { delayOn, 1.0f }, { delayLevel, 0.25f }, { reverbWet, 0.3f }
    });

    return presets;
}
