#pragma once

#include <JuceHeader.h>

struct ChainSettings
{
    float gain{0.5f};
    float reverbMix{0.5f};
    float compressMix{0.5f};
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState &apvts);

class OverVaxlingAudioProcessor : public juce::AudioProcessor
{
public:
    OverVaxlingAudioProcessor();
    ~OverVaxlingAudioProcessor() override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String &newName) override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    bool hasEditor() const override;
    juce::AudioProcessorEditor *createEditor() override;

    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{*this, nullptr, "Parameters", createParameterLayout()};

private:
    juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::Reverb, juce::dsp::Compressor<float>> processorChain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverVaxlingAudioProcessor)
};
