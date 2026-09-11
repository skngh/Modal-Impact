# Modal Impact

> [!NOTE]
> built for Wwise 2025.1.10.9233

A Wwise instrument plugin that uses modal synthesis to create procedural audio impacts.

![UI](/assets/images/UI.png)

## What it does

**Modal Impact** models what happens when you hit an object. A white noise burst acts as the **exciter**, which is shaped by an envelope and than fed into a bank of 15 biquad filters. I hand tuned each filter according to real samples I recorded.
Modal Impact models what happens when you hit a physical object. A short burst of
filtered noise stands in for the strike, and that burst excites a bank of 15 tuned
resonators that ring out the way a real object would.

Most of the samples I just recorded walking around home depot lol. Then I just used Izotope RX and its spectrum anaylzer as seen below

![Izotope](/assets/images/modes.png)

## Presets

So far there are 8 presets. I don't plan to add more in the future, but ya never know:

Metal Bottle, Knife, Ceramic Pot Big, Ceramic Pot Small, Metal Pole, Metal Rung,
Plastic Pot, Wooden Palette.

There's a randomness control that will offset the freq and ring time of each mode in the preset.
The randomness param won't change anything once the sound starts, but the transpose RTPC will.

## Controls

There's 3 groups in the plugin UI:

- **Modes** is the object itself. You can choose the preset, shape the tone with variation, length, and output level.
- **Envelope** shapes the exciter. Standard ADSR, plus the Loop switch.
- **Exciter** is a low pass on the noise burst. Shapes the tone before it hits the modes.

## Looping

Hitting the loop checkbox (or setting the param to true) prior to calling the event will make the sound loop.
This creates a "scraping" effect that may be desired. When looping, the attack and decay phase will trigger and then hold whatever value sustain is set to.
Then you can uncheck Loop (turn the RTPC to false/0), and the release phase will be triggered for the final ring.

Without looping on, each stage of the ADSR will be triggered sequentially.

## Installing

Go to Releases and download ModalImpact-bundle-2025.x.xx.x. Then in the Wwise Launcher go to plugins -> Add from directory -> choose the unzipped version of the bundle.

You can ignore package-mac and package-windows. Those are the individual packages used to make the bundle, just used in the github actions workflow.
