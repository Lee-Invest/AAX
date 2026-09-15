#include "PluginEditor.h"

namespace
{
    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text,
                      juce::Component& parent)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        parent.addAndMakeVisible(slider);

        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.attachToComponent(&slider, false);
        parent.addAndMakeVisible(label);
    }
}

SimpleVerbAudioProcessorEditor::SimpleVerbAudioProcessorEditor(SimpleVerbAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setupSlider(roomSizeSlider, roomSizeLabel, "Room Size", *this);
    setupSlider(dampingSlider, dampingLabel, "Damping", *this);
    setupSlider(wetLevelSlider, wetLevelLabel, "Wet", *this);
    setupSlider(dryLevelSlider, dryLevelLabel, "Dry", *this);
    setupSlider(widthSlider, widthLabel, "Width", *this);

    addAndMakeVisible(freezeButton);

    auto& apvts = processor.apvts;
    roomSizeAttachment = std::make_unique<SliderAttachment>(apvts, SimpleVerbAudioProcessor::roomSizeId, roomSizeSlider);
    dampingAttachment  = std::make_unique<SliderAttachment>(apvts, SimpleVerbAudioProcessor::dampingId, dampingSlider);
    wetLevelAttachment = std::make_unique<SliderAttachment>(apvts, SimpleVerbAudioProcessor::wetLevelId, wetLevelSlider);
    dryLevelAttachment = std::make_unique<SliderAttachment>(apvts, SimpleVerbAudioProcessor::dryLevelId, dryLevelSlider);
    widthAttachment    = std::make_unique<SliderAttachment>(apvts, SimpleVerbAudioProcessor::widthId, widthSlider);
    freezeAttachment   = std::make_unique<ButtonAttachment>(apvts, SimpleVerbAudioProcessor::freezeId, freezeButton);

    setSize(500, 220);
}

void SimpleVerbAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawFittedText("SimpleVerb", getLocalBounds().removeFromTop(40), juce::Justification::centred, 1);
}

void SimpleVerbAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().withTrimmedTop(50).reduced(10);
    auto knobWidth = area.getWidth() / 5;

    for (auto* slider : { &roomSizeSlider, &dampingSlider, &wetLevelSlider, &dryLevelSlider, &widthSlider })
        slider->setBounds(area.removeFromLeft(knobWidth).reduced(8, 20));

    freezeButton.setBounds(getLocalBounds().removeFromBottom(30).reduced(10, 0).removeFromLeft(100));
}
