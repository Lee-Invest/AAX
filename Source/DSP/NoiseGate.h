#pragma once
#include <juce_dsp/juce_dsp.h>

class NoiseGate
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void setParameters(float thresholdDb, float attackMs, float releaseMs);
    void process(juce::dsp::AudioBlock<float>& block);

private:
    float threshold = 0.001f;
    float attackCoeff = 0.0f, releaseCoeff = 0.0f;
    float envelope = 0.0f;
    double sampleRate = 44100.0;
};
