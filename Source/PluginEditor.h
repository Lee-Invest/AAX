#pragma once

#include "PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>

class SimpleVerbAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit SimpleVerbAudioProcessorEditor(SimpleVerbAudioProcessor&);
    ~SimpleVerbAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SimpleVerbAudioProcessor& processor;

    juce::Slider roomSizeSlider, dampingSlider, wetLevelSlider, dryLevelSlider, widthSlider;
    juce::Label roomSizeLabel, dampingLabel, wetLevelLabel, dryLevelLabel, widthLabel;
    juce::ToggleButton freezeButton { "Freeze" };

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<SliderAttachment> roomSizeAttachment, dampingAttachment,
        wetLevelAttachment, dryLevelAttachment, widthAttachment;
    std::unique_ptr<ButtonAttachment> freezeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleVerbAudioProcessorEditor)
};
