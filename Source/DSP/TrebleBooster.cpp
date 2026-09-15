#include "TrebleBooster.h"

void TrebleBooster::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    shelf.prepare(spec);
    reset();
}

void TrebleBooster::reset()
{
    shelf.reset();
}

void TrebleBooster::setParameters(float boost01, float tone01, float drive01, float outputDb)
{
    const float freq = juce::jmap(tone01, 700.0f, 2500.0f);
    const float gainDb = juce::jmap(boost01, 0.0f, 24.0f);
    *shelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate, freq, 0.5f, juce::Decibels::decibelsToGain(gainDb));

    drive = juce::jmap(drive01, 1.0f, 8.0f);
    outputGain = juce::Decibels::decibelsToGain(outputDb);
}

void TrebleBooster::process(juce::dsp::AudioBlock<float>& block)
{
    juce::dsp::ProcessContextReplacing<float> ctx(block);
    shelf.process(ctx);

    const auto numChannels = block.getNumChannels();
    const auto numSamples = block.getNumSamples();

    for (size_t ch = 0; ch < numChannels; ++ch)
    {
        auto* data = block.getChannelPointer(ch);
        for (size_t i = 0; i < numSamples; ++i)
        {
            float x = data[i] * drive;
            // asymmetric soft clip: germanium-style gentle even-harmonic saturation
            float shaped = x >= 0.0f
                ? std::tanh(x)
                : std::tanh(x * 1.3f) * 0.85f;
            data[i] = shaped * outputGain;
        }
    }
}
