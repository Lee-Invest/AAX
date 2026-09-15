#include "NoiseGate.h"

void NoiseGate::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    reset();
}

void NoiseGate::reset()
{
    envelope = 0.0f;
}

void NoiseGate::setParameters(float thresholdDb, float attackMs, float releaseMs)
{
    threshold = juce::Decibels::decibelsToGain(thresholdDb);
    attackCoeff = std::exp(-1.0f / (0.001f * attackMs * (float) sampleRate));
    releaseCoeff = std::exp(-1.0f / (0.001f * releaseMs * (float) sampleRate));
}

void NoiseGate::process(juce::dsp::AudioBlock<float>& block)
{
    const auto numChannels = block.getNumChannels();
    const auto numSamples = block.getNumSamples();

    for (size_t i = 0; i < numSamples; ++i)
    {
        float peak = 0.0f;
        for (size_t ch = 0; ch < numChannels; ++ch)
            peak = juce::jmax(peak, std::abs(block.getChannelPointer(ch)[i]));

        const bool open = peak > threshold;
        const float target = open ? 1.0f : 0.0f;
        const float coeff = open ? attackCoeff : releaseCoeff;
        envelope = target + coeff * (envelope - target);

        for (size_t ch = 0; ch < numChannels; ++ch)
            block.getChannelPointer(ch)[i] *= envelope;
    }
}
