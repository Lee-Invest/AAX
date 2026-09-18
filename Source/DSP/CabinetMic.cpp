#include "CabinetMic.h"

void CabinetMic::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    cabLowCut.prepare(spec);
    cabHighCut.prepare(spec);
    cabResonance.prepare(spec);
    micAFilter.prepare(spec);
    micBFilter.prepare(spec);
    reset();
}

void CabinetMic::reset()
{
    cabLowCut.reset();
    cabHighCut.reset();
    cabResonance.reset();
    micAFilter.reset();
    micBFilter.reset();
}

void CabinetMic::setCabinet(bool openBack, float resonance01)
{
    *cabLowCut.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 90.0f);
    *cabHighCut.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, openBack ? 6500.0f : 5500.0f);
    *cabResonance.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate, openBack ? 110.0f : 150.0f, 1.2f,
        juce::Decibels::decibelsToGain(juce::jmap(resonance01, -3.0f, 6.0f)));
}

void CabinetMic::configureMicFilter(juce::dsp::IIR::Coefficients<float>::Ptr& coeffs, const MicParams& m)
{
    // Base character per mic type, then distance/angle/position shape the curve.
    float presenceFreq = 3500.0f, presenceGainDb = 3.0f, lowFreq = 150.0f, lowGainDb = 0.0f;

    switch (m.type)
    {
        case MicType::SM57:  presenceFreq = 5000.0f; presenceGainDb = 5.0f; lowFreq = 150.0f; lowGainDb = -1.0f; break;
        case MicType::MD441: presenceFreq = 6000.0f; presenceGainDb = 2.0f; lowFreq = 100.0f; lowGainDb = 1.0f;  break;
        case MicType::U67:   presenceFreq = 8000.0f; presenceGainDb = 1.0f; lowFreq = 90.0f;  lowGainDb = 2.0f;  break;
    }

    const float proximityBoost = juce::jmap(juce::jlimit(0.5f, 30.0f, m.distanceCm), 30.0f, 0.5f, 0.0f, 6.0f);
    const float angleAttenDb = juce::jmap(m.angle01, 0.0f, 10.0f);
    const float positionTiltDb = juce::jmap(m.position01, -4.0f, 4.0f); // edge (dark) -> center (bright)

    // Mutate the existing shared Coefficients object in place. ProcessorDuplicator
    // wires its internal per-channel filters to this object once, at prepare() time;
    // reseating `coeffs` to point at a brand-new object instead would silently
    // orphan those filters (they'd keep using the old, empty default coefficients).
    *coeffs = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate, presenceFreq, 0.9f,
        juce::Decibels::decibelsToGain(presenceGainDb + positionTiltDb - angleAttenDb));

    juce::ignoreUnused(lowFreq, lowGainDb, proximityBoost);
}

void CabinetMic::setMics(const MicParams& a, const MicParams& b, float blend01)
{
    micA = a;
    micB = b;
    micBlend = blend01;

    configureMicFilter(micAFilter.state, micA);
    configureMicFilter(micBFilter.state, micB);
}

void CabinetMic::process(juce::dsp::AudioBlock<float>& block)
{
    juce::dsp::ProcessContextReplacing<float> cabCtx(block);
    cabLowCut.process(cabCtx);
    cabHighCut.process(cabCtx);
    cabResonance.process(cabCtx);

    if (!micA.enabled && !micB.enabled)
        return;

    juce::AudioBuffer<float> bufA((int) block.getNumChannels(), (int) block.getNumSamples());
    juce::AudioBuffer<float> bufB((int) block.getNumChannels(), (int) block.getNumSamples());

    for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
    {
        bufA.copyFrom((int) ch, 0, block.getChannelPointer(ch), (int) block.getNumSamples());
        bufB.copyFrom((int) ch, 0, block.getChannelPointer(ch), (int) block.getNumSamples());
    }

    juce::dsp::AudioBlock<float> blkA(bufA), blkB(bufB);
    juce::dsp::ProcessContextReplacing<float> ctxA(blkA), ctxB(blkB);

    if (micA.enabled)
        micAFilter.process(ctxA);
    if (micB.enabled)
        micBFilter.process(ctxB);

    const float gainA = micA.enabled ? micA.level * (micA.phaseInvert ? -1.0f : 1.0f) * (1.0f - micBlend) : 0.0f;
    const float gainB = micB.enabled ? micB.level * (micB.phaseInvert ? -1.0f : 1.0f) * micBlend : 0.0f;

    for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
    {
        auto* out = block.getChannelPointer(ch);
        auto* a = blkA.getChannelPointer(ch);
        auto* b = blkB.getChannelPointer(ch);
        for (size_t i = 0; i < block.getNumSamples(); ++i)
            out[i] = a[i] * gainA + b[i] * gainB;
    }
}
