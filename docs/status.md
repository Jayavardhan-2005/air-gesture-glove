# Status, known issues and notes

Last updated: 2026-11-21. Project on hold until next semester.

## Known issues
1. **Push button reads always pressed (D2).** The OLED shows PEN DOWN as soon as the board powers up. The sketch uses `INPUT_PULLUP`, so the pin should read HIGH until the button connects it to GND. Likely cause: the button is rotated 90 degrees, so two legs that are always joined sit across D2 and GND. Debug plan: pull the button out and check the display shows READY. If it does, find the always-joined leg pairs with the multimeter continuity test, then rotate the button 90 degrees and retry.
2. **DFPlayer needs 5 V.** The Nano 33 BLE's VUSB pin is not live by default (needs the solder jumper bridged). Options: bridge the jumper, power the DFPlayer from VIN when running on USB, or use a separate 5 V source. Decide when wiring audio.
3. **Glove fit.** The gloves bought are too small. Options: cut the fingertips, use velcro/straps, or buy an XL glove.
4. **Wokwi limits.** No Nano 33 BLE and no flex sensor, so simulations use an Uno and potentiometers. The sketches in `firmware/*_sim` are logic tests, not hardware-ready.

## Rev2 vs Sense Rev2
Same nRF52840 and BMI270 + BMM150 IMU, so the IMU code is identical. The Sense adds a microphone, APDS-9960 gesture/proximity/colour sensor and temperature/humidity. None are required by this project. A possible later extension is a voice trigger for mode switching.

## Design decisions
- Button as pen down/up so the model only sees deliberate gestures.
- 1 second hold in Mode 2 to avoid false alerts from small finger movements.
- Hysteresis (66% / 33%) on flex readings to stop flicker near a threshold.
- Pre-recorded voice clips on SD rather than synthesised speech, since the nRF52840 has no practical on-board TTS.
- Confidence threshold with a "try again" word instead of forcing a guess.
- Mode 2 starts as a threshold rule and is compared against a trained model so the report has a real result.

## Open questions
- Whether Mode 2 acknowledge happens by button only, or by the phone app as well.
- Whether to keep two separate models (words and caretaker) or one.
- Final phone app approach: custom app from the Stitch design, or nRF Connect for the demo.
