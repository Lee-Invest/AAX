#include "PluginProcessor.h"
#include "PluginEditor.h"

SimpleVerbAudioProcessor::SimpleVerbAudioProcessor()
    : AudioProcessor(BusesProperties()
                          .withInput("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout SimpleVerbAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        roomSizeId, "Room Size", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        dampingId, "Damping", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        wetLevelId, "Wet Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.33f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        dryLevelId, "Dry Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.4f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        widthId, "Width", juce::NormalisableRange<float>(0.0f, 1.0f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        freezeId, "Freeze", false));

    return { params.begin(), params.end() };
}

void SimpleVerbAudioProcessor::updateReverbParameters()
{
    reverbParams.roomSize   = apvts.getRawParameterValue(roomSizeId)->load();
    reverbParams.damping    = apvts.getRawParameterValue(dampingId)->load();
    reverbParams.wetLevel   = apvts.getRawParameterValue(wetLevelId)->load();
    reverbParams.dryLevel   = apvts.getRawParameterValue(dryLevelId)->load();
    reverbParams.width      = apvts.getRawParameterValue(widthId)->load();
    reverbParams.freezeMode = apvts.getRawParameterValue(freezeId)->load() > 0.5f ? 1.0f : 0.0f;

    reverb.setParameters(reverbParams);
}

void SimpleVerbAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    reverb.prepare(spec);
    updateReverbParameters();
}

void SimpleVerbAudioProcessor::releaseResources()
{
    reverb.reset();
}

bool SimpleVerbAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto mono = juce::AudioChannelSet::mono();
    const auto stereo = juce::AudioChannelSet::stereo();
    const auto out = layouts.getMainOutputChannelSet();

    if (out != mono && out != stereo)
        return false;

    return out == layouts.getMainInputChannelSet();
}

void SimpleVerbAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    updateReverbParameters();

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb.process(context);
}

juce::AudioProcessorEditor* SimpleVerbAudioProcessor::createEditor()
{
    return new SimpleVerbAudioProcessorEditor(*this);
}

void SimpleVerbAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState(); true)
    {
        std::unique_ptr<juce::XmlElement> xml(state.createXml());
        copyXmlToBinary(*xml, destData);
    }
}

void SimpleVerbAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleVerbAudioProcessor();
}
