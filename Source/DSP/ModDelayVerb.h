#pragma once
#include <juce_dsp/juce_dsp.h>

// Modulation (Foxx-style phaser + CE-1-style chorus), wah, fuzz, stereo/Echoplex
// delay (with a dedicated Brighton-Rock triple-voice wet/dry/wet mode), and reverb.
class ModDelayVerb
{
public:
    struct Params
    {
        bool phaserOn = false, chorusOn = false, wahOn = false, fuzzOn = false, delayOn = false, reverbOn = true;

        float phaserRate = 0.5f, phaserDepth = 0.5f, phaserFeedback = 0.3f;
        float chorusRate = 0.3f, chorusDepth = 0.4f, chorusMix = 0.5f;
        float wahFreq = 0.5f, wahRate = 0.4f, wahMix = 1.0f;
        float fuzzDrive = 0.5f, fuzzLevel = 0.6f;

        bool brightonRockMode = false;
        float delayTimeMs = 350.0f, delayFeedback = 0.3f, delayLevel = 0.35f;
        float brightonDelayLTimeMs = 280.0f, brightonDelayRTimeMs = 420.0f;
        float brightonFeedback = 0.35f, brightonLevel = 0.4f, brightonWidth = 1.0f, brightonPan = 0.0f;

        float reverbSize = 0.4f, reverbDamping = 0.5f, reverbWet = 0.25f, reverbWidth = 1.0f;
    };

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void setParameters(const Params& p);
    void process(juce::dsp::AudioBlock<float>& block);

private:
    juce::dsp::Phaser<float> phaser;
    juce::dsp::Chorus<float> chorus;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> wahFilter;
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;

    juce::dsp::DelayLine<float> delayMain { 192000 };
    juce::dsp::DelayLine<float> delayL { 192000 };
    juce::dsp::DelayLine<float> delayR { 192000 };

    Params params;
    float wahPhase = 0.0f;
    double sampleRate = 44100.0;
};
