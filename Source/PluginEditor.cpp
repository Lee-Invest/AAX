#include "PluginEditor.h"

namespace
{
    constexpr int knobWidth = 84, knobHeight = 90, contentWidth = 760;
    int cursorX = 0;
    int currentRowHeight = 0;
}

void BrightonRigAudioProcessorEditor::addSectionLabel(const juce::String& text)
{
    if (cursorX != 0)
    {
        layoutCursorY += currentRowHeight;
        cursorX = 0;
        currentRowHeight = 0;
    }

    layoutCursorY += 8;

    auto label = std::make_unique<juce::Label>();
    label->setText(text, juce::dontSendNotification);
    label->setFont(juce::Font(16.0f, juce::Font::bold));
    label->setColour(juce::Label::textColourId, juce::Colours::orange);
    label->setBounds(10, layoutCursorY, contentWidth - 20, 24);
    content.addAndMakeVisible(*label);
    sectionLabels.push_back(std::move(label));

    layoutCursorY += 28;
}

void BrightonRigAudioProcessorEditor::addKnob(const juce::String& paramID, const juce::String& labelText)
{
    if (cursorX + knobWidth > contentWidth)
    {
        layoutCursorY += currentRowHeight;
        cursorX = 0;
        currentRowHeight = 0;
    }
    currentRowHeight = juce::jmax(currentRowHeight, knobHeight);

    auto knob = std::make_unique<Knob>();
    knob->slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob->slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, knobWidth - 10, 16);
    knob->slider.setBounds(cursorX, layoutCursorY, knobWidth, knobHeight - 16);
    content.addAndMakeVisible(knob->slider);

    knob->label.setText(labelText, juce::dontSendNotification);
    knob->label.setFont(juce::Font(11.0f));
    knob->label.setJustificationType(juce::Justification::centred);
    knob->label.setBounds(cursorX, layoutCursorY + knobHeight - 16, knobWidth, 16);
    content.addAndMakeVisible(knob->label);

    knob->attachment = std::make_unique<SliderAttachment>(processor.apvts, paramID, knob->slider);

    cursorX += knobWidth;
    knobs.push_back(std::move(knob));
}

void BrightonRigAudioProcessorEditor::addToggle(const juce::String& paramID, const juce::String& labelText)
{
    if (cursorX + 110 > contentWidth)
    {
        layoutCursorY += currentRowHeight;
        cursorX = 0;
        currentRowHeight = 0;
    }
    currentRowHeight = juce::jmax(currentRowHeight, 30);

    auto toggle = std::make_unique<Toggle>();
    toggle->button.setButtonText(labelText);
    toggle->button.setBounds(cursorX, layoutCursorY, 110, 26);
    content.addAndMakeVisible(toggle->button);
    toggle->attachment = std::make_unique<ButtonAttachment>(processor.apvts, paramID, toggle->button);

    cursorX += 115;
    toggles.push_back(std::move(toggle));
}

BrightonRigAudioProcessorEditor::BrightonRigAudioProcessorEditor(BrightonRigAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    addAndMakeVisible(presetBox);
    presetBox.addItem("-- Select Preset --", 1);
    int idx = 2;
    for (auto& preset : processor.getPresets())
        presetBox.addItem(preset.name, idx++);
    presetBox.onChange = [this]
    {
        const int sel = presetBox.getSelectedId();
        if (sel > 1)
            processor.loadPreset(sel - 2);
    };

    addAndMakeVisible(bypassButton);
    bypassButton.setClickingTogglesState(true);
    bypassAttachment = std::make_unique<ButtonAttachment>(processor.apvts, ParamIDs::bypass, bypassButton);

    layoutCursorY = 0;
    cursorX = 0;

    addSectionLabel("Simple Mode (bypasses everything below except the amp) — turn OFF to use gate/booster/cabinet/effects");
    addToggle(ParamIDs::simpleMode, "Simple Mode");

    addSectionLabel("Noise Gate");
    addToggle(ParamIDs::gateOn, "Gate On");
    addKnob(ParamIDs::gateThreshold, "Threshold");

    addSectionLabel("Treble Booster (Rangemaster / TB-83 style)");
    addToggle(ParamIDs::boosterOn, "Booster On");
    addKnob(ParamIDs::boosterBoost, "Boost");
    addKnob(ParamIDs::boosterTone, "Tone");
    addKnob(ParamIDs::boosterDrive, "Drive");
    addKnob(ParamIDs::boosterOutput, "Output");

    addSectionLabel("Overdrive Pedal");
    addKnob(ParamIDs::odDrive, "Drive");
    addKnob(ParamIDs::odTone, "Tone");
    addKnob(ParamIDs::odLevel, "Level");

    addSectionLabel("AC30-style Amp");
    addKnob(ParamIDs::ampGain, "Gain");
    addKnob(ParamIDs::ampVolume, "Volume");
    addKnob(ParamIDs::ampBass, "Bass");
    addKnob(ParamIDs::ampMid, "Middle");
    addKnob(ParamIDs::ampTreble, "Treble");
    addKnob(ParamIDs::ampCut, "Cut");
    addKnob(ParamIDs::ampPresence, "Presence");
    addKnob(ParamIDs::ampResonance, "Resonance");
    addKnob(ParamIDs::ampPowerAmp, "Power Amp");
    addKnob(ParamIDs::ampSag, "Sag");
    addKnob(ParamIDs::ampBias, "Bias");
    addKnob(ParamIDs::ampMaster, "Master");

    addSectionLabel("Deacy Amp Blend");
    addKnob(ParamIDs::deacyBlend, "Blend");
    addKnob(ParamIDs::deacyDrive, "Drive");
    addKnob(ParamIDs::deacyTone, "Tone");
    addKnob(ParamIDs::deacyVolume, "Volume");

    addSectionLabel("Cabinet & Microphones");
    addToggle(ParamIDs::cabOpenBack, "Open Back");
    addKnob(ParamIDs::cabResonance, "Resonance");
    addKnob(ParamIDs::micABlend, "Mic A/B Blend");

    addSectionLabel("Modulation & Filter Effects");
    addToggle(ParamIDs::phaserOn, "Phaser");
    addKnob(ParamIDs::phaserRate, "Rate");
    addKnob(ParamIDs::phaserDepth, "Depth");
    addKnob(ParamIDs::phaserFeedback, "Feedback");
    addToggle(ParamIDs::chorusOn, "Chorus");
    addKnob(ParamIDs::chorusRate, "Rate");
    addKnob(ParamIDs::chorusDepth, "Depth");
    addKnob(ParamIDs::chorusMix, "Mix");
    addToggle(ParamIDs::wahOn, "Wah");
    addKnob(ParamIDs::wahFreq, "Frequency");
    addKnob(ParamIDs::wahRate, "Auto Rate");
    addKnob(ParamIDs::wahMix, "Mix");
    addToggle(ParamIDs::fuzzOn, "Fuzz");
    addKnob(ParamIDs::fuzzDrive, "Drive");
    addKnob(ParamIDs::fuzzLevel, "Level");

    addSectionLabel("Delay & Brighton Rock Stereo Mode");
    addToggle(ParamIDs::delayOn, "Delay On");
    addKnob(ParamIDs::delayTime, "Time");
    addKnob(ParamIDs::delayFeedback, "Feedback");
    addKnob(ParamIDs::delayLevel, "Level");
    addToggle(ParamIDs::brightonRockMode, "Brighton Rock");
    addKnob(ParamIDs::brightonDelayL, "Delay L");
    addKnob(ParamIDs::brightonDelayR, "Delay R");
    addKnob(ParamIDs::brightonFeedback, "Feedback");
    addKnob(ParamIDs::brightonLevel, "Level");
    addKnob(ParamIDs::brightonWidth, "Width");

    addSectionLabel("Reverb");
    addToggle(ParamIDs::reverbOn, "Reverb On");
    addKnob(ParamIDs::reverbSize, "Size");
    addKnob(ParamIDs::reverbDamping, "Damping");
    addKnob(ParamIDs::reverbWet, "Wet");

    addSectionLabel("Global");
    addKnob(ParamIDs::inputGain, "Input Gain");
    addKnob(ParamIDs::outputGain, "Output Gain");
    addKnob(ParamIDs::dryWet, "Dry/Wet");

    layoutCursorY += currentRowHeight + 20;
    content.setSize(contentWidth, layoutCursorY);

    viewport.setViewedComponent(&content, false);
    viewport.setScrollBarsShown(true, false);
    addAndMakeVisible(viewport);

    inputMeterLabel.setText("In: --", juce::dontSendNotification);
    inputMeterLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
    inputMeterLabel.setFont(juce::Font(13.0f, juce::Font::bold));
    addAndMakeVisible(inputMeterLabel);

    outputMeterLabel.setText("Out: --", juce::dontSendNotification);
    outputMeterLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
    outputMeterLabel.setFont(juce::Font(13.0f, juce::Font::bold));
    addAndMakeVisible(outputMeterLabel);

    setSize(800, 640);
    setResizable(true, true);
    startTimerHz(15);
}

void BrightonRigAudioProcessorEditor::timerCallback()
{
    auto toDbText = [](float mag, const char* prefix)
    {
        juce::String text(prefix);
        if (mag <= 0.00001f)
            return text + ": -inf dB";
        return text + ": " + juce::String(juce::Decibels::gainToDecibels(mag), 1) + " dB";
    };

    inputMeterLabel.setText(toDbText(processor.inputLevel.load(), "In"), juce::dontSendNotification);
    outputMeterLabel.setText(toDbText(processor.outputLevel.load(), "Out"), juce::dontSendNotification);
}

void BrightonRigAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1c1a17));
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawFittedText("Brighton Rig", getLocalBounds().removeFromTop(36).reduced(10, 0), juce::Justification::centredLeft, 1);
}

void BrightonRigAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    auto top = area.removeFromTop(40);
    presetBox.setBounds(top.removeFromRight(260).reduced(6));
    bypassButton.setBounds(top.removeFromRight(90).reduced(4));
    outputMeterLabel.setBounds(top.removeFromRight(110).reduced(4));
    inputMeterLabel.setBounds(top.removeFromRight(110).reduced(4));

    viewport.setBounds(area);
    content.setSize(contentWidth, content.getHeight());
}
