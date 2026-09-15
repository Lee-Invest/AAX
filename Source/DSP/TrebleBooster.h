#pragma once
#include <juce_dsp/juce_dsp.h>

// Rangemaster/TB-83 style treble booster: a high-shelf boost into a soft
// asymmetric clipper approximating a germanium transistor gain stage.
class TrebleBooster
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void setParameters(float boost01, float tone01, float drive01, float outputDb);
    void process(juce::dsp::AudioBlock<float>& block);

private:
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> shelf;
    float drive = 0.3f;
    float outputGain = 1.0f;
    double sampleRate = 44100.0;
};
