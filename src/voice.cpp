#include <cmath>
#include <array>
#include "PluginProcessor.h"
#include "voice.h"
#include "fastpower.h"

// ImpartialChoir

ImpartialChoir::ImpartialChoir()
{
	for (unsigned long i = 0; i < VOICECOUNT; ++i)
		voices[i].setSine(&lsine);
}

void ImpartialChoir::setSampleRate(double rate)
{
	for (unsigned long i = 0; i < VOICECOUNT; ++i)
		voices[i].setSampleRate(rate);
}

void ImpartialChoir::voiceOn(int noteNum, int velocity)
{
	bool openVoice = false;
	for (unsigned i = 0; (i < VOICECOUNT) && !openVoice; ++i) { // look for next off voice
		if (!(voices[(unsigned long)((nextVoice + i) % VOICECOUNT)].getOnState())) {
			nextVoice = (nextVoice + i) % VOICECOUNT;
			openVoice = true;
		}
	}

	if (!openVoice) // steal next voice
		nextVoice = (nextVoice + 1) % VOICECOUNT;
		
	voices[(unsigned long)nextVoice].noteOn(noteNum, equalTemp(noteNum), velocity);
}

void ImpartialChoir::voiceOff(int noteNum)
{
	for (unsigned long i = 0; i < VOICECOUNT; ++i) {
		if (voices[i].getMidiNote() == noteNum)
			voices[i].noteOff();
	}
}

double ImpartialChoir::equalTemp(int noteNum)
{
	return DEFAULT_REF_FREQ *
		pow(2.0, ((double)(noteNum - DEFAULT_ANCHOR_NOTE) / (double)DEFAULT_EDO));
}

void ImpartialChoir::setFreqs(double inharmBend, double inharmStretch, double inharmShift)
{
	for (unsigned long i = 0; i < VOICECOUNT; ++i)
		voices[i].setPartialFreqs(inharmBend, inharmStretch, inharmShift);
}

void ImpartialChoir::setAmps(double softness, double damping)
{
	for (unsigned long i = 0; i < VOICECOUNT; ++i)
		voices[i].setPartialAmps(softness, damping);
}

double ImpartialChoir::processVoices()
{
	double x = 0.0;
	for (unsigned long i = 0; i < VOICECOUNT; ++i)
		if (voices[i].getOnState())
			x += voices[i].process();
	return 2 * x / (double)VOICECOUNT;
}

// ImpartialVoice

ImpartialVoice::ImpartialVoice(): isOn(false), amp(0.0), pressedTime(0)
{
}

void ImpartialVoice::setSine(juce::dsp::LookupTable<double>* lsn)
{
	for (unsigned long i = 0; i < PARTIALCOUNT; ++i)
		partials[i].setSine(lsn);
}

void ImpartialVoice::setSampleRate(double rate)
{
	for (unsigned long i = 0; i < PARTIALCOUNT; ++i)
		partials[i].setSampleRate(rate);
}

void ImpartialVoice::noteOn(int note, double frequency, int velocity)
{
	isOn = true;
	pressedTime = 0;
	midiNote = note;
	freq = frequency;
	amp = (double)velocity / 127.0;
}

void ImpartialVoice::noteOff()
{
	isOn = false;
	amp = 0.0;
}

int ImpartialVoice::getMidiNote()
{
	return midiNote;
}

bool ImpartialVoice::getOnState()
{
	return isOn;
}

void ImpartialVoice::setPartialFreqs(double inharmBend, double inharmStretch,
		double inharmShift)
{
	for (unsigned long i = 0; i < PARTIALCOUNT; ++i) {
		partials[i].setRelFreq((fastPow(i + 1, fastPow(2.0, inharmBend)) - 1)
			* fastPow(2, inharmStretch) * freq / (inharmShift + freq) + 1);
	}
}

void ImpartialVoice::setPartialAmps(double softness, double damping)
{
	double inv = 1.5 * log(2.0 * (softness + 0.5) / 3.0) / log(2.0) + 1.0;
	double expDamp = fastPow(2, damping - 14.0);
	for (unsigned long i = 0; i < PARTIALCOUNT; ++i) {
		partials[i].setRelAmp(fastPow(((double)i + (1 / inv)) * inv, -softness)
			//* fastPow(10, -expDamp * expDamp * i * i * pressedTime / sampleRate * 1000));	
			);
	}
}

double ImpartialVoice::process()
{
	double x = 0.0;
	if (isOn || (amp > 1.0 / (double)0x100000000)) {
		for (unsigned long i = 0; i < PARTIALCOUNT; ++i)
			x += partials[i].process(freq, amp);
		++pressedTime;
	}
	return x;
}

// ImpartialPartial

ImpartialPartial::ImpartialPartial(): lsine(nullptr), relFreq(1.0), relAmp(1.0), phase(0.0)
{
}

void ImpartialPartial::setSine(juce::dsp::LookupTable<double>* lsn)
{
	lsine = lsn;
}

void ImpartialPartial::setSampleRate(double rate)
{
	sampleRate = rate;
}

void ImpartialPartial::setRelFreq(double freq)
{
	relFreq = freq;
}

void ImpartialPartial::setRelAmp(double amp)
{
	relAmp = amp;
}

double ImpartialPartial::process(double baseFreq, double baseAmp)
{
	phase += baseFreq * relFreq / sampleRate;
	while (phase >= 1.0)
		phase -= 1.0;
	if (baseFreq * relFreq <= sampleRate / 2) // prevent aliasing from super high freqs
		return lsine->get(phase * (double)SINELEN) * baseAmp * relAmp;
	return 0;
}
