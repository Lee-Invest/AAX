#pragma once
#include <juce_dsp/juce_dsp.h>

// Two-stage overdrive (generic pedal) + AC30-style preamp/tonestack/power-amp,
// plus an optional Deacy-style low-gain amp path that can be blended in.
class AmpStage
{
public:
    struct Params
    {
        float overdriveDrive = 0.0f, overdriveTone = 0.5f, overdriveLevel = 0.5f;

        float gain = 0.5f, volume = 0.5f;
        float bass = 0.5f, mid = 0.5f, treble = 0.5f, cut = 0.3f;
        float presence = 0.5f, resonance = 0.5f;
        float powerAmp = 0.6f, sag = 0.4f, bias = 0.5f, master = 0.7f;

        float deacyBlend = 0.0f, deacyDrive = 0.3f, deacyTone = 0.5f, deacyVolume = 0.5f;
    };

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void setParameters(const Params& p);
    void process(juce::dsp::AudioBlock<float>& block);

private:
    float shapeStage(float x, float driveAmount) const;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> odTone;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> bassFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> midFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> trebleFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> cutFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> presenceFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> deacyTone;

    Params params;
    float sagEnvelope = 0.0f;
    double sampleRate = 44100.0;
};
