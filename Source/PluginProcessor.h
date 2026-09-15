#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "DSP/NoiseGate.h"
#include "DSP/TrebleBooster.h"
#include "DSP/AmpStage.h"
#include "DSP/CabinetMic.h"
#include "DSP/ModDelayVerb.h"
#include "Presets.h"

namespace ParamIDs
{
    // Gate
    static const juce::String gateThreshold = "gateThreshold";
    static const juce::String gateOn = "gateOn";

    // Treble booster
    static const juce::String boosterOn = "boosterOn";
    static const juce::String boosterBoost = "boosterBoost";
    static const juce::String boosterTone = "boosterTone";
    static const juce::String boosterDrive = "boosterDrive";
    static const juce::String boosterOutput = "boosterOutput";

    // Overdrive (pedal, feeds into amp)
    static const juce::String odDrive = "odDrive";
    static const juce::String odTone = "odTone";
    static const juce::String odLevel = "odLevel";

    // AC30 Amp
    static const juce::String ampGain = "ampGain";
    static const juce::String ampVolume = "ampVolume";
    static const juce::String ampBass = "ampBass";
    static const juce::String ampMid = "ampMid";
    static const juce::String ampTreble = "ampTreble";
    static const juce::String ampCut = "ampCut";
    static const juce::String ampPresence = "ampPresence";
    static const juce::String ampResonance = "ampResonance";
    static const juce::String ampPowerAmp = "ampPowerAmp";
    static const juce::String ampSag = "ampSag";
    static const juce::String ampBias = "ampBias";
    static const juce::String ampMaster = "ampMaster";

    // Deacy
    static const juce::String deacyBlend = "deacyBlend";
    static const juce::String deacyDrive = "deacyDrive";
    static const juce::String deacyTone = "deacyTone";
    static const juce::String deacyVolume = "deacyVolume";

    // Cabinet / mics
    static const juce::String cabOpenBack = "cabOpenBack";
    static const juce::String cabResonance = "cabResonance";
    static const juce::String micABlend = "micABlend";

    // Effects
    static const juce::String phaserOn = "phaserOn";
    static const juce::String phaserRate = "phaserRate";
    static const juce::String phaserDepth = "phaserDepth";
    static const juce::String phaserFeedback = "phaserFeedback";

    static const juce::String chorusOn = "chorusOn";
    static const juce::String chorusRate = "chorusRate";
    static const juce::String chorusDepth = "chorusDepth";
    static const juce::String chorusMix = "chorusMix";

    static const juce::String wahOn = "wahOn";
    static const juce::String wahFreq = "wahFreq";
    static const juce::String wahRate = "wahRate";
    static const juce::String wahMix = "wahMix";

    static const juce::String fuzzOn = "fuzzOn";
    static const juce::String fuzzDrive = "fuzzDrive";
    static const juce::String fuzzLevel = "fuzzLevel";

    static const juce::String delayOn = "delayOn";
    static const juce::String delayTime = "delayTime";
    static const juce::String delayFeedback = "delayFeedback";
    static const juce::String delayLevel = "delayLevel";

    static const juce::String brightonRockMode = "brightonRockMode";
    static const juce::String brightonDelayL = "brightonDelayL";
    static const juce::String brightonDelayR = "brightonDelayR";
    static const juce::String brightonFeedback = "brightonFeedback";
    static const juce::String brightonLevel = "brightonLevel";
    static const juce::String brightonWidth = "brightonWidth";

    static const juce::String reverbOn = "reverbOn";
    static const juce::String reverbSize = "reverbSize";
    static const juce::String reverbDamping = "reverbDamping";
    static const juce::String reverbWet = "reverbWet";

    // Global
    static const juce::String inputGain = "inputGain";
    static const juce::String outputGain = "outputGain";
    static const juce::String dryWet = "dryWet";
    static const juce::String bypass = "bypass";
}

class BrightonRigAudioProcessor : public juce::AudioProcessor
{
public:
    BrightonRigAudioProcessor();
    ~BrightonRigAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 4.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void loadPreset(int index);
    const std::vector<PresetInfo>& getPresets() const { return presets; }

    juce::AudioProcessorValueTreeState apvts;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    NoiseGate gate;
    TrebleBooster booster;
    AmpStage amp;
    CabinetMic cabinet;
    ModDelayVerb fx;

    std::vector<PresetInfo> presets;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BrightonRigAudioProcessor)
};
