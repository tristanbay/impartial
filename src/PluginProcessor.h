#pragma once
#include <JuceHeader.h>
#include <cmath>
#include "voice.h"

// parameter stuff

#define P_SOFTNESS_L 1.0
#define P_SOFTNESS_U 2.5
#define P_SOFTNESS_D 1.25

#define P_INHRMBND_L -1.25
#define P_INHRMBND_U 1.25
#define P_INHRMBND_D 0.0

#define P_INHRMSTR_L -2
#define P_INHRMSTR_U 2
#define P_INHRMSTR_D 0.0

#define P_INHRMSHF_L -20
#define P_INHRMSHF_U 20
#define P_INHRMSHF_D 0.0

#define P_DAMPING_L 0
#define P_DAMPING_U 8.0
#define P_DAMPING_D 3.0

struct PluginAudioProcessor: public juce::AudioProcessor
{
	PluginAudioProcessor();
	~PluginAudioProcessor() override;
	
	void prepareToPlay(double, int) override;
	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
 	void releaseResources() override;

	#ifndef JucePlugin_PreferredChannelConfigurations
		bool isBusesLayoutSupported(const BusesLayout&) const override;
	#endif

	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int) override;
	const juce::String getProgramName(int) override;
	void changeProgramName(int, const juce::String&) override;

	void getStateInformation(juce::MemoryBlock&) override;
	void setStateInformation(const void*, int) override;
	private:
		// plugin parameters
		juce::AudioParameterFloat* softness,
								 * inharmBend,
								 * inharmStretch,
								 * inharmShift,
								 * damping;

		ImpartialChoir choir; // the audio engine

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginAudioProcessor)
};
