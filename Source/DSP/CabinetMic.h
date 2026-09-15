#pragma once
#include <juce_dsp/juce_dsp.h>

// Cabinet + up-to-two-microphone EQ approximation (no IR convolution — uses
// filter stacks tuned to approximate AC30 2x12 / Alnico Blue + SM57/MD441/U67
// character and mic distance/proximity/phase/blend behaviour).
class CabinetMic
{
public:
    enum class MicType { SM57, MD441, U67 };

    struct MicParams
    {
        bool enabled = true;
        MicType type = MicType::SM57;
        float position01 = 0.5f;   // cone edge -> center
        float distanceCm = 5.0f;
        float angle01 = 0.0f;      // on-axis -> off-axis
        bool phaseInvert = false;
        float level = 1.0f;
    };

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void setCabinet(bool openBack, float resonance01);
    void setMics(const MicParams& a, const MicParams& b, float blend01);
    void process(juce::dsp::AudioBlock<float>& block);

private:
    void configureMicFilter(juce::dsp::IIR::Coefficients<float>::Ptr& coeffs, const MicParams& m);

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> cabLowCut, cabHighCut, cabResonance;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> micAFilter, micBFilter;

    MicParams micA, micB;
    float micBlend = 0.0f;
    double sampleRate = 44100.0;
};
