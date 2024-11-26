#include "PluginProcessor.h"
#include "PluginEditor.h"

OverVaxlingAudioProcessor::OverVaxlingAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

OverVaxlingAudioProcessor::~OverVaxlingAudioProcessor() {}

const juce::String OverVaxlingAudioProcessor::getName() const { return JucePlugin_Name; }
bool OverVaxlingAudioProcessor::acceptsMidi() const { return false; }
bool OverVaxlingAudioProcessor::producesMidi() const { return false; }
bool OverVaxlingAudioProcessor::isMidiEffect() const { return false; }
double OverVaxlingAudioProcessor::getTailLengthSeconds() const { return 0.5; }
int OverVaxlingAudioProcessor::getNumPrograms() { return 1; }
int OverVaxlingAudioProcessor::getCurrentProgram() { return 0; }
void OverVaxlingAudioProcessor::setCurrentProgram(int index) {}
const juce::String OverVaxlingAudioProcessor::getProgramName(int index) { return {}; }
void OverVaxlingAudioProcessor::changeProgramName(int index, const juce::String &newName) {}

void OverVaxlingAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    processorChain.prepare(spec);
}

void OverVaxlingAudioProcessor::releaseResources() {}

bool OverVaxlingAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet() && (layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono() || layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo());
}

void OverVaxlingAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &)
{
    juce::ScopedNoDenormals noDenormals;

    auto chainSettings = getChainSettings(apvts);

    auto &gainProcessor = processorChain.get<0>();
    gainProcessor.setGainLinear(chainSettings.gain * 2);

    auto &reverbProcessor = processorChain.get<1>();
    juce::dsp::Reverb::Parameters reverbParams;
    reverbParams.roomSize = chainSettings.reverbMix;
    reverbParams.wetLevel = chainSettings.reverbMix;
    reverbProcessor.setParameters(reverbParams);

    auto &compressor = processorChain.get<2>();
    float sliderValue = chainSettings.compressMix;
    float threshold = -20.0f + (sliderValue * 0.2f);
    float ratio = 1.0f + std::log10(1.0f + sliderValue);
    float attack = 0.5f + (sliderValue * 0.05f);
    float release = 50.0f + (sliderValue * sliderValue * 0.2f);

    compressor.setThreshold(threshold);
    compressor.setRatio(ratio);
    compressor.setAttack(attack);
    compressor.setRelease(release);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    processorChain.process(context);
}

bool OverVaxlingAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor *OverVaxlingAudioProcessor::createEditor() { return new OverVaxlingAudioProcessorEditor(*this); }

void OverVaxlingAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    juce::MemoryOutputStream stream(destData, true);
    apvts.state.writeToStream(stream);
}

void OverVaxlingAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
        apvts.replaceState(tree);
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState &apvts)
{
    ChainSettings settings;
    settings.gain = apvts.getRawParameterValue("Gain")->load();
    settings.reverbMix = apvts.getRawParameterValue("ReverbMix")->load();
    settings.compressMix = apvts.getRawParameterValue("Compressor")->load();

    return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout OverVaxlingAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("Gain", "Fa et hjerte i en lydfil", 0.0f, 50.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("ReverbMix", "Rumklang skal maxes", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Compressor", "Comp skal maxes", 0.0f, 50.0f, 0.5f));

    return layout;
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new OverVaxlingAudioProcessor();
}
