#pragma once
#include <JuceHeader.h>
#include <array>

#define VOICECOUNT 8 // number of voices for synth
#define PARTIALCOUNT 64 // number of partials per voice
#define SINELEN 65536 // sample length of sine wave lookup table
#define DEFAULT_EDO 41 // default tuning (EDO) of instrument
#define DEFAULT_ANCHOR_NOTE 67 // default note to set the reference frequency to
#define DEFAULT_REF_FREQ 440.0 // default reference frequency
#define PI juce::MathConstants<double>::pi

struct ImpartialPartial // structure for single partial
{
	ImpartialPartial();

	void setSine(juce::dsp::LookupTable<double>*);
	void setSampleRate(double);

	void setRelFreq(double);
	void setRelAmp(double);

	double process(double, double); // return audio sample, inputs are base freq and amp

	private:
		double relFreq; // frequency of partial in terms of base voice frequency
		double relAmp;	// amplitude of partial in terms of base voice amplitude
		double phase; // phase of partial's waveform

		double sampleRate; // copy of sample rate held for local use
		juce::dsp::LookupTable<double>* lsine; // pointer to waveform lookup table
};

struct ImpartialVoice // structure for single voice
{
	ImpartialVoice();

	void setSine(juce::dsp::LookupTable<double>*);
	void setSampleRate(double);

	void noteOn(int, double, int); // what to do when note on message
	void noteOff(); // what to do when note off message

	int getMidiNote(); // get the MIDI assigned to the voice
	bool getOnState(); // get if the note is on or off

	void setPartialFreqs(double, double, double); // control voice's inharmonicity of timbre
	void setPartialAmps(double, double); // control voice's "filtering" of timbre

	double process(); // audio processing of voice if on, returns sample

	private:
		bool isOn; // is the voice in the "on" state?
		unsigned long pressedTime; // time in samples since pressed
		int midiNote; // assigned MIDI note of voice

		double freq; // frequency of voice in hz
		double amp; // amplitude of voice from 0 to 1
		double phase; // phase of voice's waveform

		double sampleRate; // copy of sample rate held for local use

		std::array<ImpartialPartial, PARTIALCOUNT> partials; // individual partials
};

struct ImpartialChoir // structure for all voices, envelopes, etc. in synth
{
	ImpartialChoir();

	void setSampleRate(double);

	void voiceOn(int, int); // turn on next voice
	void voiceOff(int); // search for voices with matching MIDI note and turn them off

	void setFreqs(double, double, double); // control inharmonicity of timbre globally
	void setAmps(double, double); // control "filtering" of timbre globally

	double processVoices(); // calls process function for each voice and adds them together

	double equalTemp(int); // MIDI note number to specified edo + reference note and pitch

	private:
		unsigned nextVoice; // which voice to play next

		// the lookup table used for calculating the sine waveform
		juce::dsp::LookupTable<double> lsine = juce::dsp::LookupTable<double>(
			[](double x) { return sin(2.0 * PI / (double)SINELEN * x) / 2.0; }, SINELEN);


		std::array<ImpartialVoice, VOICECOUNT> voices; // the individual voices
};
