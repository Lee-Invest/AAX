#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace ParamIDs;

BrightonRigAudioProcessor::BrightonRigAudioProcessor()
    : AudioProcessor(BusesProperties()
                          .withInput("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    presets = getFactoryPresets();
}

juce::AudioProcessorValueTreeState::ParameterLayout BrightonRigAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    auto addFloat = [&p](const juce::String& id, const juce::String& name, float lo, float hi, float def)
    {
        p.push_back(std::make_unique<juce::AudioParameterFloat>(id, name, juce::NormalisableRange<float>(lo, hi), def));
    };
    auto addBool = [&p](const juce::String& id, const juce::String& name, bool def)
    {
        p.push_back(std::make_unique<juce::AudioParameterBool>(id, name, def));
    };

    addBool(gateOn, "Noise Gate On", true);
    addFloat(gateThreshold, "Gate Threshold", -80.0f, -20.0f, -50.0f);

    addBool(boosterOn, "Treble Booster On", true);
    addFloat(boosterBoost, "Booster Boost", 0.0f, 1.0f, 0.7f);
    addFloat(boosterTone, "Booster Tone", 0.0f, 1.0f, 0.6f);
    addFloat(boosterDrive, "Booster Drive", 0.0f, 1.0f, 0.4f);
    addFloat(boosterOutput, "Booster Output", -12.0f, 12.0f, 0.0f);

    addFloat(odDrive, "Overdrive Drive", 0.0f, 1.0f, 0.0f);
    addFloat(odTone, "Overdrive Tone", 0.0f, 1.0f, 0.5f);
    addFloat(odLevel, "Overdrive Level", 0.0f, 1.0f, 0.5f);

    addFloat(ampGain, "Amp Gain", 0.0f, 1.0f, 0.6f);
    addFloat(ampVolume, "Amp Volume", 0.0f, 1.0f, 0.6f);
    addFloat(ampBass, "Amp Bass", 0.0f, 1.0f, 0.5f);
    addFloat(ampMid, "Amp Middle", 0.0f, 1.0f, 0.6f);
    addFloat(ampTreble, "Amp Treble", 0.0f, 1.0f, 0.65f);
    addFloat(ampCut, "Amp Cut", 0.0f, 1.0f, 0.6f);
    addFloat(ampPresence, "Amp Presence", 0.0f, 1.0f, 0.5f);
    addFloat(ampResonance, "Amp Resonance", 0.0f, 1.0f, 0.4f);
    addFloat(ampPowerAmp, "Power Amp", 0.0f, 1.0f, 0.6f);
    addFloat(ampSag, "Sag", 0.0f, 1.0f, 0.4f);
    addFloat(ampBias, "Bias", 0.0f, 1.0f, 0.5f);
    addFloat(ampMaster, "Master", 0.0f, 1.0f, 0.7f);

    addFloat(deacyBlend, "Deacy Blend", 0.0f, 1.0f, 0.0f);
    addFloat(deacyDrive, "Deacy Drive", 0.0f, 1.0f, 0.3f);
    addFloat(deacyTone, "Deacy Tone", 0.0f, 1.0f, 0.5f);
    addFloat(deacyVolume, "Deacy Volume", 0.0f, 1.0f, 0.5f);

    addBool(cabOpenBack, "Open Back Cabinet", false);
    addFloat(cabResonance, "Cab Resonance", 0.0f, 1.0f, 0.4f);
    addFloat(micABlend, "Mic A/B Blend", 0.0f, 1.0f, 0.3f);

    addBool(phaserOn, "Phaser On", false);
    addFloat(phaserRate, "Phaser Rate", 0.0f, 1.0f, 0.4f);
    addFloat(phaserDepth, "Phaser Depth", 0.0f, 1.0f, 0.5f);
    addFloat(phaserFeedback, "Phaser Feedback", 0.0f, 1.0f, 0.3f);

    addBool(chorusOn, "Chorus On", false);
    addFloat(chorusRate, "Chorus Rate", 0.0f, 1.0f, 0.3f);
    addFloat(chorusDepth, "Chorus Depth", 0.0f, 1.0f, 0.4f);
    addFloat(chorusMix, "Chorus Mix", 0.0f, 1.0f, 0.5f);

    addBool(wahOn, "Wah On", false);
    addFloat(wahFreq, "Wah Frequency", 0.0f, 1.0f, 0.5f);
    addFloat(wahRate, "Wah Rate", 0.0f, 1.0f, 0.4f);
    addFloat(wahMix, "Wah Mix", 0.0f, 1.0f, 1.0f);

    addBool(fuzzOn, "Fuzz On", false);
    addFloat(fuzzDrive, "Fuzz Drive", 0.0f, 1.0f, 0.5f);
    addFloat(fuzzLevel, "Fuzz Level", 0.0f, 1.0f, 0.6f);

    addBool(delayOn, "Delay On", true);
    addFloat(delayTime, "Delay Time (ms)", 50.0f, 1500.0f, 350.0f);
    addFloat(delayFeedback, "Delay Feedback", 0.0f, 0.95f, 0.3f);
    addFloat(delayLevel, "Delay Level", 0.0f, 1.0f, 0.3f);

    addBool(brightonRockMode, "Brighton Rock Mode", false);
    addFloat(brightonDelayL, "Brighton Delay L (ms)", 50.0f, 1500.0f, 280.0f);
    addFloat(brightonDelayR, "Brighton Delay R (ms)", 50.0f, 1500.0f, 420.0f);
    addFloat(brightonFeedback, "Brighton Feedback", 0.0f, 0.95f, 0.35f);
    addFloat(brightonLevel, "Brighton Level", 0.0f, 1.0f, 0.4f);
    addFloat(brightonWidth, "Brighton Width", 0.0f, 1.5f, 1.0f);

    addBool(reverbOn, "Reverb On", true);
    addFloat(reverbSize, "Reverb Size", 0.0f, 1.0f, 0.4f);
    addFloat(reverbDamping, "Reverb Damping", 0.0f, 1.0f, 0.5f);
    addFloat(reverbWet, "Reverb Wet", 0.0f, 1.0f, 0.25f);

    addFloat(inputGain, "Input Gain", -24.0f, 24.0f, 0.0f);
    addFloat(outputGain, "Output Gain", -24.0f, 24.0f, 0.0f);
    addFloat(dryWet, "Dry/Wet", 0.0f, 1.0f, 1.0f);
    addBool(bypass, "Bypass", false);

    return { p.begin(), p.end() };
}

void BrightonRigAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
    spec.numChannels = (juce::uint32) getTotalNumOutputChannels();

    gate.prepare(spec);
    booster.prepare(spec);
    amp.prepare(spec);
    cabinet.prepare(spec);
    fx.prepare(spec);
}

void BrightonRigAudioProcessor::releaseResources() {}

bool BrightonRigAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo())
        return false;
    return out == layouts.getMainInputChannelSet();
}

void BrightonRigAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    if (*apvts.getRawParameterValue(bypass) > 0.5f)
        return;

    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);

    juce::dsp::AudioBlock<float> block(buffer);

    const float inGain = juce::Decibels::decibelsToGain(apvts.getRawParameterValue(inputGain)->load());
    block.multiplyBy(inGain);

    if (*apvts.getRawParameterValue(gateOn) > 0.5f)
    {
        gate.setParameters(apvts.getRawParameterValue(gateThreshold)->load(), 2.0f, 120.0f);
        gate.process(block);
    }

    if (*apvts.getRawParameterValue(boosterOn) > 0.5f)
    {
        booster.setParameters(apvts.getRawParameterValue(boosterBoost)->load(),
                               apvts.getRawParameterValue(boosterTone)->load(),
                               apvts.getRawParameterValue(boosterDrive)->load(),
                               apvts.getRawParameterValue(boosterOutput)->load());
        booster.process(block);
    }

    AmpStage::Params ap;
    ap.overdriveDrive = apvts.getRawParameterValue(odDrive)->load();
    ap.overdriveTone = apvts.getRawParameterValue(odTone)->load();
    ap.overdriveLevel = apvts.getRawParameterValue(odLevel)->load();
    ap.gain = apvts.getRawParameterValue(ampGain)->load();
    ap.volume = apvts.getRawParameterValue(ampVolume)->load();
    ap.bass = apvts.getRawParameterValue(ampBass)->load();
    ap.mid = apvts.getRawParameterValue(ampMid)->load();
    ap.treble = apvts.getRawParameterValue(ampTreble)->load();
    ap.cut = apvts.getRawParameterValue(ampCut)->load();
    ap.presence = apvts.getRawParameterValue(ampPresence)->load();
    ap.resonance = apvts.getRawParameterValue(ampResonance)->load();
    ap.powerAmp = apvts.getRawParameterValue(ampPowerAmp)->load();
    ap.sag = apvts.getRawParameterValue(ampSag)->load();
    ap.bias = apvts.getRawParameterValue(ampBias)->load();
    ap.master = apvts.getRawParameterValue(ampMaster)->load();
    ap.deacyBlend = apvts.getRawParameterValue(deacyBlend)->load();
    ap.deacyDrive = apvts.getRawParameterValue(deacyDrive)->load();
    ap.deacyTone = apvts.getRawParameterValue(deacyTone)->load();
    ap.deacyVolume = apvts.getRawParameterValue(deacyVolume)->load();
    amp.setParameters(ap);
    amp.process(block);

    cabinet.setCabinet(*apvts.getRawParameterValue(cabOpenBack) > 0.5f, apvts.getRawParameterValue(cabResonance)->load());
    CabinetMic::MicParams micA, micB;
    micA.type = CabinetMic::MicType::SM57;
    micB.type = CabinetMic::MicType::MD441;
    cabinet.setMics(micA, micB, apvts.getRawParameterValue(micABlend)->load());
    cabinet.process(block);

    ModDelayVerb::Params fxp;
    fxp.phaserOn = *apvts.getRawParameterValue(phaserOn) > 0.5f;
    fxp.phaserRate = apvts.getRawParameterValue(phaserRate)->load();
    fxp.phaserDepth = apvts.getRawParameterValue(phaserDepth)->load();
    fxp.phaserFeedback = apvts.getRawParameterValue(phaserFeedback)->load();
    fxp.chorusOn = *apvts.getRawParameterValue(chorusOn) > 0.5f;
    fxp.chorusRate = apvts.getRawParameterValue(chorusRate)->load();
    fxp.chorusDepth = apvts.getRawParameterValue(chorusDepth)->load();
    fxp.chorusMix = apvts.getRawParameterValue(chorusMix)->load();
    fxp.wahOn = *apvts.getRawParameterValue(wahOn) > 0.5f;
    fxp.wahFreq = apvts.getRawParameterValue(wahFreq)->load();
    fxp.wahRate = apvts.getRawParameterValue(wahRate)->load();
    fxp.wahMix = apvts.getRawParameterValue(wahMix)->load();
    fxp.fuzzOn = *apvts.getRawParameterValue(fuzzOn) > 0.5f;
    fxp.fuzzDrive = apvts.getRawParameterValue(fuzzDrive)->load();
    fxp.fuzzLevel = apvts.getRawParameterValue(fuzzLevel)->load();
    fxp.delayOn = *apvts.getRawParameterValue(delayOn) > 0.5f;
    fxp.delayTimeMs = apvts.getRawParameterValue(delayTime)->load();
    fxp.delayFeedback = apvts.getRawParameterValue(delayFeedback)->load();
    fxp.delayLevel = apvts.getRawParameterValue(delayLevel)->load();
    fxp.brightonRockMode = *apvts.getRawParameterValue(brightonRockMode) > 0.5f;
    fxp.brightonDelayLTimeMs = apvts.getRawParameterValue(brightonDelayL)->load();
    fxp.brightonDelayRTimeMs = apvts.getRawParameterValue(brightonDelayR)->load();
    fxp.brightonFeedback = apvts.getRawParameterValue(brightonFeedback)->load();
    fxp.brightonLevel = apvts.getRawParameterValue(brightonLevel)->load();
    fxp.brightonWidth = apvts.getRawParameterValue(brightonWidth)->load();
    fxp.reverbOn = *apvts.getRawParameterValue(reverbOn) > 0.5f;
    fxp.reverbSize = apvts.getRawParameterValue(reverbSize)->load();
    fxp.reverbDamping = apvts.getRawParameterValue(reverbDamping)->load();
    fxp.reverbWet = apvts.getRawParameterValue(reverbWet)->load();
    fx.setParameters(fxp);
    fx.process(block);

    const float outGain = juce::Decibels::decibelsToGain(apvts.getRawParameterValue(outputGain)->load());
    block.multiplyBy(outGain);

    const float wet = apvts.getRawParameterValue(dryWet)->load();
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* out = buffer.getWritePointer(ch);
        auto* dry = dryBuffer.getReadPointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            out[i] = dry[i] * (1.0f - wet) + out[i] * wet;
    }
}

void BrightonRigAudioProcessor::loadPreset(int index)
{
    if (index < 0 || index >= (int) presets.size())
        return;

    for (auto& [id, value] : presets[(size_t) index].values)
        if (auto* param = apvts.getParameter(id))
            param->setValueNotifyingHost(param->convertTo0to1(value));
}

juce::AudioProcessorEditor* BrightonRigAudioProcessor::createEditor()
{
    return new BrightonRigAudioProcessorEditor(*this);
}

void BrightonRigAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void BrightonRigAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BrightonRigAudioProcessor();
}
