#pragma once

struct ImpartialEnvelope
{
	ImpartialEnvelope();
	private:
		double attack, decay, sustain, release; // ADSR values
		double progress; // attack: 0 - 1, decay: 1 - 2, release: 2 - 3
		double envValue; // output value of the envelope
};
