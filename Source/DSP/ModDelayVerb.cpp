#include "ModDelayVerb.h"

void ModDelayVerb::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    phaser.prepare(spec);
    chorus.prepare(spec);
    wahFilter.prepare(spec);
    reverb.prepare(spec);
    delayMain.prepare(spec);
    delayL.prepare(spec);
    delayR.prepare(spec);
    reset();
}

void ModDelayVerb::reset()
{
    phaser.reset();
    chorus.reset();
    wahFilter.reset();
    reverb.reset();
    delayMain.reset();
    delayL.reset();
    delayR.reset();
    wahPhase = 0.0f;
}

void ModDelayVerb::setParameters(const Params& p)
{
    params = p;

    phaser.setRate(juce::jmap(p.phaserRate, 0.1f, 8.0f));
    phaser.setDepth(p.phaserDepth);
    phaser.setFeedback(p.phaserFeedback * 0.9f);
    phaser.setCentreFrequency(1000.0f);
    phaser.setMix(p.phaserOn ? 0.6f : 0.0f);

    chorus.setRate(juce::jmap(p.chorusRate, 0.05f, 5.0f));
    chorus.setDepth(p.chorusDepth);
    chorus.setCentreDelay(7.0f);
    chorus.setFeedback(0.15f);
    chorus.setMix(p.chorusOn ? p.chorusMix : 0.0f);

    reverbParams.roomSize = p.reverbSize;
    reverbParams.damping = p.reverbDamping;
    reverbParams.wetLevel = p.reverbOn ? p.reverbWet : 0.0f;
    reverbParams.dryLevel = 1.0f;
    reverbParams.width = p.reverbWidth;
    reverb.setParameters(reverbParams);

    delayMain.setDelay((float) (p.delayTimeMs * 0.001 * sampleRate));
    delayL.setDelay((float) (p.brightonDelayLTimeMs * 0.001 * sampleRate));
    delayR.setDelay((float) (p.brightonDelayRTimeMs * 0.001 * sampleRate));
}

void ModDelayVerb::process(juce::dsp::AudioBlock<float>& block)
{
    const auto numChannels = block.getNumChannels();
    const auto numSamples = block.getNumSamples();

    // Fuzz
    if (params.fuzzOn)
    {
        const float d = juce::jmap(params.fuzzDrive, 2.0f, 20.0f);
        for (size_t ch = 0; ch < numChannels; ++ch)
        {
            auto* data = block.getChannelPointer(ch);
            for (size_t i = 0; i < numSamples; ++i)
            {
                float x = data[i] * d;
                float clipped = juce::jlimit(-1.0f, 1.0f, x);
                data[i] = clipped * params.fuzzLevel;
            }
        }
    }

    // Auto-wah: LFO-swept bandpass (coefficients updated once per block)
    if (params.wahOn)
    {
        const float rate = juce::jmap(params.wahRate, 0.2f, 6.0f);
        wahPhase += rate * (float) numSamples / (float) sampleRate;
        if (wahPhase > 1.0f) wahPhase -= std::floor(wahPhase);

        const float sweep = 0.5f + 0.5f * std::sin(wahPhase * juce::MathConstants<float>::twoPi);
        const float freq = juce::jmap(params.wahFreq * sweep, 300.0f, 2200.0f);
        *wahFilter.state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, freq, 0.7f);

        juce::AudioBuffer<float> dryBuf((int) numChannels, (int) numSamples);
        for (size_t ch = 0; ch < numChannels; ++ch)
            dryBuf.copyFrom((int) ch, 0, block.getChannelPointer(ch), (int) numSamples);

        juce::dsp::ProcessContextReplacing<float> wahCtx(block);
        wahFilter.process(wahCtx);

        for (size_t ch = 0; ch < numChannels; ++ch)
        {
            auto* wet = block.getChannelPointer(ch);
            auto* dry = dryBuf.getReadPointer((int) ch);
            for (size_t i = 0; i < numSamples; ++i)
                wet[i] = dry[i] * (1.0f - params.wahMix) + wet[i] * params.wahMix;
        }
    }

    // Modulation
    if (params.phaserOn)
    {
        juce::dsp::ProcessContextReplacing<float> ctx(block);
        phaser.process(ctx);
    }
    if (params.chorusOn)
    {
        juce::dsp::ProcessContextReplacing<float> ctx(block);
        chorus.process(ctx);
    }

    // Delay
    if (params.delayOn)
    {
        if (params.brightonRockMode && numChannels >= 2)
        {
            auto* left = block.getChannelPointer(0);
            auto* right = block.getChannelPointer(1);

            for (size_t i = 0; i < numSamples; ++i)
            {
                const float dryMid = 0.5f * (left[i] + right[i]);

                delayL.pushSample(0, dryMid + delayL.popSample(0) * params.brightonFeedback);
                delayR.pushSample(0, dryMid + delayR.popSample(0) * params.brightonFeedback);
                const float wetL = delayL.popSample(0);
                const float wetR = delayR.popSample(0);

                const float width = params.brightonWidth;
                left[i] = dryMid + wetL * params.brightonLevel * width;
                right[i] = dryMid + wetR * params.brightonLevel * width;
            }
        }
        else
        {
            for (size_t ch = 0; ch < numChannels; ++ch)
            {
                auto* data = block.getChannelPointer(ch);
                for (size_t i = 0; i < numSamples; ++i)
                {
                    const float delayed = delayMain.popSample((int) ch);
                    delayMain.pushSample((int) ch, data[i] + delayed * params.delayFeedback);
                    data[i] = data[i] + delayed * params.delayLevel;
                }
            }
        }
    }

    // Reverb (always processed with wet=0 when off, cheap no-op-ish)
    juce::dsp::ProcessContextReplacing<float> revCtx(block);
    reverb.process(revCtx);
}
