# Air-Gesture Assistive Glove

A wearable **Embedded AI** project built on the Arduino Nano 33 BLE family. A glove with motion and bend sensors turns hand movements into words, shown on a display and spoken aloud. A second mode lets a bed-ridden or paralysed patient call a caretaker with just a few finger movements.

> **Status: on hold until next semester.** Hardware bring-up is done; the ML model, audio, BLE app and glove assembly are still to do. See [Status](#status) for exactly what works and what doesn't.

Built by Jayavardhana A. (B.Tech AI & ML, Ballari Institute of Technology and Management) for the Embedded AI course.

---

## What it does

The system follows the standard embedded-AI loop: **sensors → edge AI → decision → actuators → feedback**.

### Mode 1: Air-gesture pen (communication)
1. Hold a push button (pen down).
2. Draw a shape in the air with your hand.
3. Release the button (pen up).
4. The IMU samples recorded between press and release are classified on the board by a TinyML model.
5. The recognised shape maps to a word, which is shown on the OLED and spoken through a speaker.

| Shape drawn | Word |
|---|---|
| Circle | hello |
| Check mark | yes |
| Slash | no |
| Triangle | help |
| Zig-zag wave | water |
| (low confidence) | try again |

### Mode 2: Caretaker mode (patient alerts)
Three flex sensors on the index, middle and ring fingers detect which fingers are bent. The pattern has to be held for 1 second to count, which filters out accidental movement.

| Fingers bent | Message |
|---|---|
| Index only | HUNGRY |
| Index + middle | WASHROOM |
| Index + middle + ring | EMERGENCY |

The alert shows on the display, plays the matching voice clip, and the buzzer and vibration motor keep going until the caretaker acknowledges (a button for now, the phone app later). EMERGENCY beeps faster and higher.

Plan: benchmark a simple threshold rule against a trained model for this mode and report both.

### Phone app (planned)
A BLE link to a phone app to switch modes, mirror the recognised text live, and acknowledge alerts. For demos a generic BLE app (nRF Connect) is the fallback. UI design is generated with Google Stitch.

---

## Architecture

```
 Mode 1                                              Outputs
 ┌──────────┐   ┌───────────────┐   ┌─────────────┐   ┌──────────────┐
 │ IMU      │──▶│ windowing +   │──▶│ TinyML      │──▶│ OLED (word)  │
 │ (accel + │   │ filtering     │   │ classifier  │   │ Speaker      │
 │  gyro)   │   └───────────────┘   │ (Edge       │   │ (DFPlayer)   │
 │ + button │                       │  Impulse)   │   │ Buzzer       │
 └──────────┘                       └─────────────┘   │ Vibration    │
                                           │          └──────────────┘
 Mode 2                                    ▼                 ▲
 ┌──────────┐   ┌───────────────┐   ┌─────────────┐          │
 │ 3x flex  │──▶│ ADC + percent │──▶│ pattern +   │──────────┘
 │ sensors  │   │ + hysteresis  │   │ 1 s hold    │
 └──────────┘   └───────────────┘   └─────────────┘
                                           ▲
                         BLE phone app ────┘ (mode switch, acknowledge)
```

---

## Hardware

### Boards
| Board | Role |
|---|---|
| Arduino Nano 33 BLE **Rev2** | Current development board. No mic or gesture sensor. |
| Arduino Nano 33 BLE **Sense Rev2** | Arriving next semester. Same MCU (nRF52840) and IMU, plus microphone, APDS-9960 and temperature/humidity. The project does not depend on the extra sensors, so code moves over directly. |

Both run at 3.3 V logic. **Pins are not 5 V tolerant.** The 5 V pin (VUSB) is only live when the jumper is bridged, which matters for the DFPlayer (see Known issues).

### Bill of materials
| Part | Purpose |
|---|---|
| Arduino Nano 33 BLE Rev2 (Sense Rev2 later) | MCU, BLE, on-board IMU (BMI270 + BMM150) |
| 3x flex sensors | Finger bend detection (Mode 2) |
| Fixed resistors (22k / 33k range) | Voltage dividers for the flex sensors |
| SSD1306 0.96" OLED, I2C (0x3C) | Shows the recognised word |
| DFPlayer Mini + microSD card | Plays pre-recorded voice clips |
| 8 Ω 0.5 W 40 mm speaker | Voice output |
| Active buzzer module | Alert tone |
| Coin vibration motor | Haptic alert |
| BC547 NPN transistor, 1 kΩ base resistor, 1N4007 diode | Motor driver with flyback protection |
| Push button | Pen down / pen up, acknowledge |
| Breadboard, jumper wires | Prototyping |
| Glove | Wearable body (fit still to be sorted) |

### Measured flex sensor values
| Bend | Resistance |
|---|---|
| Flat | about 19.9 kΩ |
| Gentle curve | about 23.2 kΩ |
| Tight bend | about 48 kΩ |

The sensor sits in a voltage divider with a fixed resistor feeding an analog pin. Calibration records flat and bent readings per finger, then maps to a percentage. A finger counts as bent above 66% and released below 33% (hysteresis).

### Motor driver
Coin motor measured about 96 Ω (roughly 35 mA at 3.3 V). Driven through a BC547 with a 1 kΩ base resistor, with a 1N4007 across the motor as flyback protection.

### Current wiring (breadboard)
Rows counted from the USB end. Left column is the Nano's left pin row, right column its right pin row.

| Row | Left pin | Right pin |
|---|---|---|
| 1 | D13 | D12 |
| 2 | 3V3 | D11 |
| 3 | REF | D10 |
| 4 | A0 | D9 |
| 5 | A1 | D8 |
| 6 | A2 | D7 |
| 7 | A3 | D6 |
| 8 | A4 (SDA) | D5 |
| 9 | A5 (SCL) | D4 |
| 10 | A6 | D3 |
| 11 | A7 | D2 |
| 12 | VUSB | GND |
| 13 | RST | RST |
| 14 | GND | RX0 |
| 15 | VIN | TX1 |

OLED (I2C): GND to the ground rail, VDD to the 3V3 rail, SCK to A5, SDA to A4.

Planned audio file layout on the SD card (FAT32, `/MP3` folder): `0001` hello, `0002` yes, `0003` no, `0004` help, `0005` water, `0006` hungry, `0007` washroom, `0008` emergency, `0009` try again.

---

## Software stack

| Tool / library | Use |
|---|---|
| Arduino IDE 2.x | Firmware development |
| Arduino Mbed OS Nano Boards core | Board support (select "Arduino Nano 33 BLE") |
| `Arduino_BMI270_BMM150` | IMU driver for the Rev2 boards (the older Rev1 uses LSM9DS1 and a different library) |
| `Adafruit_SSD1306` + `Adafruit_GFX` | OLED display |
| DFPlayer library | Voice playback (planned) |
| `ArduinoBLE` | Phone link (planned) |
| Edge Impulse | Data collection, training and deployment of the gesture model (planned) |
| Wokwi | Logic prototyping. Wokwi has no Nano 33 BLE and no flex sensor, so logic was tested on an Uno with potentiometers standing in |
| Google Stitch | Phone app UI design (planned) |

### ML approach (planned)
- Record labelled gesture windows from the IMU between button press and release, from several sessions and hand speeds.
- Train a small classifier in Edge Impulse (spectral features or 1D-CNN) and export it as an Arduino library.
- Add a confidence threshold: below it the glove says "try again" rather than guessing.
- For Mode 2, compare the plain threshold rule against a trained model and report accuracy and latency of both.

---

## Repository layout

```
firmware/
  oled_hello/              OLED "HELLO" test (hardware)
  pen_button_oled/         button -> PEN DOWN / PEN UP on the OLED (hardware)
  pen_button_buzzer_sim/   pen down/up logic with buzzer (Wokwi, Uno)
  flex_calibration/        flex calibration + percent mapping (Wokwi, Uno)
  caretaker_mode_sim/      3-finger pattern logic, hold time, alerts (Wokwi, Uno)
docs/
  status.md                what is done, what is not, known issues, roadmap
```

---

## Status

**Working (verified on the bench)**
- Board bring-up, Blink, live IMU readings (about 100 Hz)
- OLED wired and displaying text
- Vibration motor and speaker soldered and tested
- Flex sensors characterised with a multimeter
- Flex calibration, hysteresis and 3-finger pattern logic, **simulated in Wokwi only**

**Not done yet**
- Push button on D2 reads permanently pressed (unresolved, see [docs/status.md](docs/status.md))
- Gesture data collection, model training and on-device inference
- Voice clips and DFPlayer wiring
- BLE phone app
- Glove assembly and fit

Nothing here has been tested end to end on the glove, and no accuracy figures exist yet.

## Roadmap
1. Fix the button fault, then verify pen-down/pen-up on hardware.
2. Move to the Sense Rev2 board.
3. Write the data-collection sketch, record the five gestures, train in Edge Impulse.
4. Run inference on the board and wire the result to display and speech.
5. Record voice clips, wire the DFPlayer.
6. Mount flex sensors on the glove and port Mode 2 from the simulator to hardware.
7. BLE app for mode switch, text mirror and acknowledge.
8. Evaluate and write up: accuracy, confusion matrix, latency, threshold vs model.

## Reference projects
- [OmarShafei-coder/Sign-Language-Glove](https://github.com/OmarShafei-coder/Sign-Language-Glove)
- [Panizghi/Sign-Language-Translator](https://github.com/Panizghi/Sign-Language-Translator)
- [VeronicaGupta/RTOS_Sensor_Fusion_Stack](https://github.com/VeronicaGupta/RTOS_Sensor_Fusion_Stack)
