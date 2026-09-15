#include "AmpStage.h"

void AmpStage::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    odTone.prepare(spec);
    bassFilter.prepare(spec);
    midFilter.prepare(spec);
    trebleFilter.prepare(spec);
    cutFilter.prepare(spec);
    presenceFilter.prepare(spec);
    deacyTone.prepare(spec);
    reset();
}

void AmpStage::reset()
{
    odTone.reset();
    bassFilter.reset();
    midFilter.reset();
    trebleFilter.reset();
    cutFilter.reset();
    presenceFilter.reset();
    deacyTone.reset();
    sagEnvelope = 0.0f;
}

void AmpStage::setParameters(const Params& p)
{
    params = p;

    *odTone.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate, 1200.0f, 0.6f, juce::Decibels::decibelsToGain(juce::jmap(p.overdriveTone, -6.0f, 10.0f)));

    *bassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        sampleRate, 120.0f, 0.7f, juce::Decibels::decibelsToGain(juce::jmap(p.bass, -12.0f, 12.0f)));

    *midFilter.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate, 700.0f, 0.8f, juce::Decibels::decibelsToGain(juce::jmap(p.mid, -12.0f, 12.0f)));

    *trebleFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate, 3000.0f, 0.7f, juce::Decibels::decibelsToGain(juce::jmap(p.treble, -12.0f, 12.0f)));

    *cutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
        sampleRate, juce::jmap(p.cut, 2000.0f, 12000.0f));

    *presenceFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate, 4500.0f, 0.7f, juce::Decibels::decibelsToGain(juce::jmap(p.presence, 0.0f, 10.0f)));

    *deacyTone.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
        sampleRate, juce::jmap(p.deacyTone, 1200.0f, 6000.0f));
}

float AmpStage::shapeStage(float x, float driveAmount) const
{
    const float d = 1.0f + driveAmount * 9.0f;
    return std::tanh(x * d) / std::tanh(d);
}

void AmpStage::process(juce::dsp::AudioBlock<float>& block)
{
    const auto numChannels = block.getNumChannels();
    const auto numSamples = block.getNumSamples();

    // Overdrive pedal stage
    if (params.overdriveDrive > 0.001f)
    {
        for (size_t ch = 0; ch < numChannels; ++ch)
        {
            auto* data = block.getChannelPointer(ch);
            for (size_t i = 0; i < numSamples; ++i)
                data[i] = shapeStage(data[i], params.overdriveDrive) * juce::jmap(params.overdriveLevel, 0.3f, 1.5f);
        }
        juce::dsp::ProcessContextReplacing<float> odCtx(block);
        odTone.process(odCtx);
    }

    // AC30-style preamp gain stage with sag-driven compression
    for (size_t i = 0; i < numSamples; ++i)
    {
        float peak = 0.0f;
        for (size_t ch = 0; ch < numChannels; ++ch)
            peak = juce::jmax(peak, std::abs(block.getChannelPointer(ch)[i]));

        const float sagTarget = peak;
        const float sagCoeff = 0.0005f + params.sag * 0.02f;
        sagEnvelope += (sagTarget - sagEnvelope) * sagCoeff;
        const float compGain = 1.0f / (1.0f + sagEnvelope * params.sag * 2.0f);

        const float preGain = juce::jmap(params.gain, 0.5f, 6.0f);
        const float biasAsym = juce::jmap(params.bias, -0.15f, 0.15f);

        for (size_t ch = 0; ch < numChannels; ++ch)
        {
            auto* data = block.getChannelPointer(ch);
            float in = data[i] * preGain * compGain + biasAsym;
            float shaped = shapeStage(in, 0.6f + params.gain * 0.4f);
            data[i] = shaped - biasAsym * 0.5f;
        }
    }

    juce::dsp::ProcessContextReplacing<float> tsCtx(block);
    bassFilter.process(tsCtx);
    midFilter.process(tsCtx);
    trebleFilter.process(tsCtx);
    cutFilter.process(tsCtx);
    presenceFilter.process(tsCtx);

    // Power-amp stage: second, softer saturation for harmonic complexity + sustain
    const float powerDrive = juce::jmap(params.powerAmp, 0.2f, 2.0f);
    for (size_t ch = 0; ch < numChannels; ++ch)
    {
        auto* data = block.getChannelPointer(ch);
        for (size_t i = 0; i < numSamples; ++i)
        {
            float x = data[i] * powerDrive;
            data[i] = std::tanh(x) * juce::jmap(params.volume, 0.0f, 1.4f) * params.master;
        }
    }

    // Deacy-style low-gain amp blended in for that thick, dark rhythm layer
    if (params.deacyBlend > 0.001f)
    {
        juce::dsp::AudioBlock<float> deacyBlock(block);
        for (size_t ch = 0; ch < numChannels; ++ch)
        {
            auto* data = deacyBlock.getChannelPointer(ch);
            for (size_t i = 0; i < numSamples; ++i)
            {
                float x = data[i] * juce::jmap(params.deacyDrive, 0.5f, 3.0f);
                data[i] = std::tanh(x) * params.deacyVolume;
            }
        }
        juce::dsp::ProcessContextReplacing<float> deacyCtx(deacyBlock);
        deacyTone.process(deacyCtx);

        for (size_t ch = 0; ch < numChannels; ++ch)
        {
            auto* dry = block.getChannelPointer(ch);
            auto* wet = deacyBlock.getChannelPointer(ch);
            for (size_t i = 0; i < numSamples; ++i)
                dry[i] = dry[i] * (1.0f - params.deacyBlend) + wet[i] * params.deacyBlend;
        }
    }
}
