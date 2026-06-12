# Impartial: an inharmonic additive synthesizer plugin
This is an audio plugin I've been working on, but it's still in development.

## Current features
- 8-voice polyphony
- 64 partials per voice that can be detuned away from the harmonic series
- A control to make the timbre less bright by making higher partials quieter
- Generic JUCE GUI

## Things I'd like the plugin to have in the future
- Multiple ADSR (or more complex) envelopes per voice
- LFOs
- Retunability without having to change pre-processor macros and recompile
- More ways to manipulate the partials
- More partials per voice
- A faster/more accurate fast power function (?)
- Mono legato mode
- Variable number of voices
- Custom GUI

## Libraries/other code used
[JUCE](https://juce.com) under the GNU Affero General Public License version 3

## License
Copyright 2026 Tristan Bay, licensed under the GNU Affero General Public License version 3 (see `license.txt`)
