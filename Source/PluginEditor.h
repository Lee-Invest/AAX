#pragma once

#include "PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>

class BrightonRigAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit BrightonRigAudioProcessorEditor(BrightonRigAudioProcessor&);
    ~BrightonRigAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    struct Knob
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<SliderAttachment> attachment;
    };

    struct Toggle
    {
        juce::ToggleButton button;
        std::unique_ptr<ButtonAttachment> attachment;
    };

    void addKnob(const juce::String& paramID, const juce::String& labelText);
    void addToggle(const juce::String& paramID, const juce::String& labelText);
    void addSectionLabel(const juce::String& text);

    BrightonRigAudioProcessor& processor;

    juce::Viewport viewport;
    juce::Component content;

    std::vector<std::unique_ptr<Knob>> knobs;
    std::vector<std::unique_ptr<Toggle>> toggles;
    std::vector<std::unique_ptr<juce::Label>> sectionLabels;

    juce::ComboBox presetBox;
    juce::TextButton bypassButton { "Bypass" };
    std::unique_ptr<ButtonAttachment> bypassAttachment;

    int layoutCursorY = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BrightonRigAudioProcessorEditor)
};
