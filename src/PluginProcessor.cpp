#include "PluginProcessor.h"
#include "voice.h"

PluginAudioProcessor::PluginAudioProcessor()
	#ifndef JucePlugin_PreferredChannelConfigurations
		: AudioProcessor(BusesProperties()
			#if ! JucePlugin_IsMidiEffect
				#if ! JucePlugin_IsSynth
					.withInput("Input", juce::AudioChannelSet::stereo(), true)
				#endif
				.withOutput("Output", juce::AudioChannelSet::stereo(), true)
			#endif
		)
	#endif
{
	addParameter(softness = new juce::AudioParameterFloat(
				"softness", "Timbre Softness",
				juce::NormalisableRange<float>(P_SOFTNESS_L, P_SOFTNESS_U), P_SOFTNESS_D));
	addParameter(inharmBend = new juce::AudioParameterFloat(
				"inharmBend", "Inharmonic Bend",
				juce::NormalisableRange<float>(P_INHRMBND_L, P_INHRMBND_U), P_INHRMBND_D));
	addParameter(inharmStretch = new juce::AudioParameterFloat(
				"inharmStretch", "Inharmonic Stretch",
				juce::NormalisableRange<float>(P_INHRMSTR_L, P_INHRMSTR_U), P_INHRMSTR_D));
	addParameter(inharmShift = new juce::AudioParameterFloat(
				"inharmShift", "Inharmonic Shift",
				juce::NormalisableRange<float>(P_INHRMSHF_L, P_INHRMSHF_U), P_INHRMSHF_D));
	addParameter(damping = new juce::AudioParameterFloat(
				"damping", "Damping Amount",
				juce::NormalisableRange<float>(P_DAMPING_L, P_DAMPING_U), P_DAMPING_D));
}

PluginAudioProcessor::~PluginAudioProcessor()
{
}

void PluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	juce::ignoreUnused(sampleRate, samplesPerBlock);
	choir.setSampleRate(sampleRate);
}

void PluginAudioProcessor::processBlock
		(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	auto bufLength = buffer.getNumSamples();

	// clear audio out buffer
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, bufLength);
	
	// send MIDI messages to voices
	for (const auto midi : midiMessages) {
		auto msg = midi.getMessage();
		if (msg.isNoteOn()) {
			choir.voiceOn(msg.getNoteNumber(), msg.getVelocity());
		} else {
			choir.voiceOff(msg.getNoteNumber());
		}
	}

	// apply params to audio engine
		choir.setAmps(*softness, *damping);
		choir.setFreqs(*inharmBend, *inharmStretch, *inharmShift);

	// process audio
	float temp = 0.0;
	for (int i = 0; i < bufLength; ++i) {
		temp = choir.processVoices();
		for (int j = 0; j < totalNumOutputChannels; ++j)
			buffer.setSample(j, i, temp); // put audio into output channels
	}
}

void PluginAudioProcessor::releaseResources()
{
	// use this to free memory when playback stopped
}

#ifndef JucePlugin_PreferredChannelConfigurations
	bool PluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
	{
		#if JucePlugin_IsMidiEffect
			juce::ignoreUnused(layouts);
			return true;
		#else
			if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
					&& layouts.getMainOutputChannelSet()
					!= juce::AudioChannelSet::stereo()) {
				return false;
			}
			#if ! JucePlugin_IsSynth
				if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
					return false;
			#endif
			return true;
		#endif
	}
#endif

juce::AudioProcessorEditor* PluginAudioProcessor::createEditor()
{
	return new juce::GenericAudioProcessorEditor(*this);
}

bool PluginAudioProcessor::hasEditor() const
{
	return true;
}

const juce::String PluginAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool PluginAudioProcessor::acceptsMidi() const
{
	#if JucePlugin_WantsMidiInput
		return true;
	#else
		return false;
	#endif
}

bool PluginAudioProcessor::producesMidi() const
{
	#if JucePlugin_ProducesMidiInput
		return true;
	#else
		return false;
	#endif
}

bool PluginAudioProcessor::isMidiEffect() const
{
	#if JucePlugin_IsMidiEffect
		return true;
	#else
		return false;
	#endif
}

double PluginAudioProcessor::getTailLengthSeconds() const
{
	return 0;
}

int PluginAudioProcessor::getNumPrograms()
{
	return 1;
}

int PluginAudioProcessor::getCurrentProgram()
{
	return 0;
}

void PluginAudioProcessor::setCurrentProgram(int index)
{
	juce::ignoreUnused(index);
}

const juce::String PluginAudioProcessor::getProgramName(int index)
{
	juce::ignoreUnused(index);
	return {};
}

void PluginAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
	juce::ignoreUnused(index, newName);
}

void PluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	std::unique_ptr<juce::XmlElement> stateInfo(new juce::XmlElement("Impartial"));
	stateInfo->setAttribute("softness", (double)*softness);
	stateInfo->setAttribute("inharmBend", (double)*inharmBend);
	stateInfo->setAttribute("inharmStretch", (double)*inharmStretch);
	stateInfo->setAttribute("inharmShift", (double)*inharmShift);
	stateInfo->setAttribute("damping", (double)*damping);
	copyXmlToBinary(*stateInfo, destData);
}

void PluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	std::unique_ptr<juce::XmlElement> stateInfo(getXmlFromBinary(data, sizeInBytes));
	if (stateInfo.get() != nullptr) {
		if (stateInfo->hasTagName("Impartial")) {
			*softness = (float)stateInfo->getDoubleAttribute("softness", P_SOFTNESS_D);
			*inharmBend = (float)stateInfo->getDoubleAttribute("inharmBend", P_INHRMBND_D);
			*inharmStretch = (float)stateInfo->getDoubleAttribute("inharmStretch",
					P_INHRMSTR_D);
			*inharmShift = (float)stateInfo->getDoubleAttribute("inharmShift",
					P_INHRMSHF_D);
			*damping = (float)stateInfo->getDoubleAttribute("damping",
					P_DAMPING_D);
		}
	}
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	// creates new plugin instances
	return new PluginAudioProcessor();
}
