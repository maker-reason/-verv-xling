/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct CustomSlider : juce::Slider
{
  CustomSlider()
      : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                     juce::Slider::TextEntryBoxPosition::NoTextBox)
  {
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(228u, 197u, 175u));
    setColour(juce::Slider::thumbColourId, juce::Colour(249u, 110u, 70u));
    setColour(juce::Slider::trackColourId, juce::Colour(158u, 206u, 154u));
  }
};
//==============================================================================
/**
 */
class OverVaxlingAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
  OverVaxlingAudioProcessorEditor(OverVaxlingAudioProcessor &);
  ~OverVaxlingAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

private:
  void timerCallback() override;
  OverVaxlingAudioProcessor &audioProcessor;
  std::vector<juce::Component *> getComps();
  std::vector<juce::Point<float>> dotPositions;

  CustomSlider gainSlider, reverbSlider, compressorSlider;
  juce::Label gainLabel, reverbLabel, compressorLabel;
  using APVTS = juce::AudioProcessorValueTreeState;
  using Attachment = APVTS::SliderAttachment;

  Attachment gainSliderAttachment,
      reverbSliderAttachment,
      compressorSliderAttachment;
  bool isIncreasing = true;
  juce::Image backgroundImage;

  juce::ColourGradient backgroundGradient;
  float gradientPosition = 0.0f;
  juce::Random random;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverVaxlingAudioProcessorEditor)
};
