/*
 ==============================================================================

 This file contains the basic framework code for a JUCE plugin editor.

 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OverVaxlingAudioProcessorEditor::OverVaxlingAudioProcessorEditor(OverVaxlingAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      gainSliderAttachment(audioProcessor.apvts, "Gain", gainSlider),
      reverbSliderAttachment(audioProcessor.apvts, "ReverbMix", reverbSlider),
      compressorSliderAttachment(audioProcessor.apvts, "Compressor", compressorSlider)
{
    for (auto *comp : getComps())
    {
        addAndMakeVisible(comp);
    }
    gainLabel.setText("Gearskifte for helvede ", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.attachToComponent(&gainSlider, false);
    addAndMakeVisible(gainLabel);

    reverbLabel.setText("Rumklaeng", juce::dontSendNotification);
    reverbLabel.setJustificationType(juce::Justification::centred);
    reverbLabel.attachToComponent(&reverbSlider, false);
    addAndMakeVisible(reverbLabel);

    compressorLabel.setText("Turb0 K0mpress0r", juce::dontSendNotification);
    compressorLabel.setJustificationType(juce::Justification::centred);
    compressorLabel.attachToComponent(&compressorSlider, false);
    addAndMakeVisible(compressorLabel);
    for (int i = 0; i < 50; ++i)
    {
        dotPositions.push_back(juce::Point<float>(random.nextFloat() * 600.0f, random.nextFloat() * 300.0f));
    }

    startTimer(20);
    setSize(600, 300);
}

OverVaxlingAudioProcessorEditor::~OverVaxlingAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void OverVaxlingAudioProcessorEditor::paint(juce::Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    juce::ColourGradient gradient(juce::Colour(14u, 71u, 73u), 150.0f, 150.0f,
                                  juce::Colour(158u, 206u, 154u), 600.0f, 300.0f,
                                  false);
    g.setGradientFill(backgroundGradient);

    g.fillAll();

    juce::File bundle = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                            .getParentDirectory()
                            .getParentDirectory();

    g.setColour(juce::Colour(16u, 16u, 12u));

    for (const auto &dot : dotPositions)
    {
        g.fillEllipse(dot.x, dot.y, 10.0f, 10.0f);
    }
    g.setColour(juce::Colour(116u, 165u, 127u));
    g.setFont(juce::FontOptions(35.0f));
    auto textBounds = getLocalBounds().reduced(20);
    g.drawFittedText("Efterspil og mig",
                     textBounds, juce::Justification::centredTop, 4);
}
void OverVaxlingAudioProcessorEditor::timerCallback()
{

    if (isIncreasing)
    {
        gradientPosition += 0.01f;
        if (gradientPosition >= 1.0f)
        {
            gradientPosition = 1.0f;
            isIncreasing = false;
        }
    }
    else
    {
        gradientPosition -= 0.01f;
        if (gradientPosition <= 0.0f)
        {
            gradientPosition = 0.0f;
            isIncreasing = true;
        }
    }

    float startX = 150.0f + 250.0f * std::sin(gradientPosition * juce::MathConstants<float>::pi);
    float startY = 150.0f + 250.0f * std::cos(gradientPosition * juce::MathConstants<float>::pi);
    float endX = 600.0f + 250.0f * std::sin(gradientPosition * juce::MathConstants<float>::pi);
    float endY = 300.0f + 250.0f * std::cos(gradientPosition * juce::MathConstants<float>::pi);

    backgroundGradient = juce::ColourGradient(
        juce::Colour(14u, 71u, 73u), startX, startY,
        juce::Colour(158u, 206u, 154u), endX, endY,
        false);
    for (auto &dot : dotPositions)
    {
        dot.x += random.nextFloat() * 4.0f - 2.0f;
        dot.y += random.nextFloat() * 4.0f - 2.0f;

        if (dot.x < 0)
            dot.x = 0;
        if (dot.x > 600)
            dot.x = 600;
        if (dot.y < 0)
            dot.y = 0;
        if (dot.y > 300)
            dot.y = 300;
    }
    repaint();
}
void OverVaxlingAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(100);

    int sliderHeight = 100;
    int labelHeight = 20;
    auto sliderArea = bounds.removeFromTop(sliderHeight);
    auto totalWidth = sliderArea.getWidth();
    auto sliderWidth = totalWidth / 3;

    auto gainArea = sliderArea.removeFromLeft(sliderWidth);
    auto reverbArea = sliderArea.removeFromLeft(sliderWidth);
    auto compressArea = sliderArea.removeFromLeft(sliderWidth);

    gainSlider.setBounds(gainArea);
    reverbSlider.setBounds(reverbArea);
    compressorSlider.setBounds(compressArea);

    auto gainLabelArea = gainArea.withHeight(labelHeight).translated(0, sliderHeight);
    auto reverbLabelArea = reverbArea.withHeight(labelHeight).translated(0, sliderHeight);
    auto compressorLabelArea = compressArea.withHeight(labelHeight).translated(0, sliderHeight);

    gainLabel.setBounds(gainLabelArea);
    reverbLabel.setBounds(reverbLabelArea);
    compressorLabel.setBounds(compressorLabelArea);

    float fontSize = sliderWidth * 0.1f;
    gainLabel.setFont(juce::Font(fontSize));
    reverbLabel.setFont(juce::Font(fontSize));
    compressorLabel.setFont(juce::Font(fontSize));
}

std::vector<juce::Component *> OverVaxlingAudioProcessorEditor::getComps()
{
    return {
        &gainSlider,
        &reverbSlider,
        &compressorSlider};
}
