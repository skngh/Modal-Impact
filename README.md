# Modal Impact

A Wwise source plug-in that synthesizes impact sounds. No samples, no audio files.
Every hit is generated from scratch at runtime.

<!-- TODO: screenshot of the plug-in UI in Wwise -->

## What it does

Modal Impact models what happens when you hit a physical object. A short burst of
filtered noise stands in for the strike, and that burst excites a bank of 15 tuned
resonators that ring out the way a real object would.

The resonator settings come from presets measured off actual objects, so a ceramic
pot sounds like a ceramic pot and a metal pole sounds like a metal pole. Because
nothing is pre-recorded, you get a slightly different hit every time and never run
into the repetition you'd get from cycling through a handful of wav files.

## Presets

Eight objects ship with the plug-in:

Metal Bottle, Knife, Ceramic Pot Big, Ceramic Pot Small, Metal Pole, Metal Rung,
Plastic Pot, Wooden Palette.

The Randomness control decides how far each hit is allowed to drift from the
preset. At 0 every impact is identical. Turn it up and each one gets its own
character while still sounding like the same object.

## Controls

Grouped into three sections in the plug-in UI:

- **Modes** is the object itself. Which preset, how much variation, tuning, ring
  length and output level.
- **Envelope** shapes the strike. Standard ADSR, plus the Loop switch.
- **Exciter** is a low pass on the noise going in. Roll it down for a softer,
  duller hit.

Every control has its own help text inside Wwise. Click a property and hit F1.

## Looping

This is the part worth knowing about, because it does something slightly unusual.

With Loop off you get a one shot. The envelope fires once and the plug-in works
out its own duration from the attack, decay, release and the longest ringing mode,
so the voice stops on its own once the sound has actually finished. You don't have
to guess at a length.

With Loop on the envelope holds at its sustain level instead of releasing, and the
object gets driven continuously rather than struck once. Think scraping or rattling
instead of a single impact.

The useful bit is that Loop is an RTPC, so you can flip it mid sound. Turn it off
while the voice is playing and the plug-in triggers the release, recalculates how
long the tail needs, and lets the resonators ring out naturally before stopping.
So you can hold a continuous rattle for as long as you need and then release it
into a proper decay, all from one event.

## Installing

<!-- TODO: fill in once you've decided how you're distributing this -->

Grab the bundle zip from the releases page and point the Wwise Launcher at it.

## License

<!-- TODO: pick one -->
