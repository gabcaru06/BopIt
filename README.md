# BopIt (Elder Scrolls Edition)


Speak It! is an Arduino based reimagining of "Bop It," built around the
persuasion mechanics from The Elder Scrolls III: Morrowind. Rather than
pressing buttons, the player negotiates with the device across three
physical inputs: dropping a coin into a slot to **Bribe**, shaking the box
to **Intimidate**, and speaking or making noise to **Admire**. A library of
sound cues tells the player which response is needed each round: a gold
coin clinking calls for a bribe, an angry "bark" calls for intimidation,
and a happy "bark" calls for admiration. Each round must be completed
correctly and quickly using the matching sensor, and the response window
shortens as the score climbs, ramping up the difficulty.

## Project structure

```
BopIt/
├── bop_it.ino              # Main game sketch (the file to upload to play the game)
├── Test Sketches/          # Standalone sketches used to test each piece of hardware individually
│   ├── TestAccel.ino       # Accelerometer test (Intimidate)
│   ├── TestHexDisplay.ino  # 2-digit 7-segment score display test
│   ├── TestMic.ino         # Microphone/sound sensor test (Admire)
│   ├── TestMP3.ino         # DFPlayer Mini MP3 module test
│   └── TestPhoto.ino       # InfraredS sensor test (Bribe)
└── README.md
```

## Main file of interest: `bop_it.ino`

This is the only sketch that needs to be uploaded to play the game. It's a
single self contained `.ino` file organized into these sections:

- **Pin definitions & constants**: all hardware pins, action IDs, sensor
  thresholds, and the 7-segment digit lookup table are defined at the top of
  the file so the wiring/calibration can be adjusted in one place.
- **Hardware init (`initializeHardware`)**: configures pin modes, starts
  serial/I2C communication, and wakes/verifies the accelerometer.
- **Sensor read functions**: `readAccelerator()`, `readMicrophone()`, and
  `readPhotoSensor()` each return a reading used to check the player's action.
- **Hex display functions**: `updateHexDisplay()` / `writeTwoDigits()` drive
  the 7-segment display through the chained 74HC595 shift registers.
- **Game functions**: `getRandomAction()`, `playInstruction()`,
  `waitForAction()`, and `decreaseResponseTime()` implement the core game
  loop: prompt a random action, wait for the matching response, and
  progressively reduce the response window, called from `setup()`/`loop()`.

### Pin map

| Signal | Pin | Notes |
|---|---|---|
| Microphone (Admire) | `A1` | Sampled over a 50ms window; peak-to-peak amplitude compared to `micThreshold` |
| Infrared/light sensor (Bribe) | `A2` | Compared to `photoThreshold` |
| Accelerometer (Intimidate) | I2C (`A4`/`A5`) | MPU6050 at address `0x68`; reads the X-axis register over `Wire` |
| Green LED | `2` | Correct-action feedback |
| Red LED | `3` | Wrong-action/timeout feedback |
| Start button | `7` | `INPUT_PULLUP`, active LOW |
| Hex display serial/clock/latch | `4` / `6` / `5` | Drives the two chained 74HC595 shift registers |
| DFPlayer Mini (MP3) | `0` / `1` | Sends play-track commands over serial |

### Actions

| Action | Physical input | Audio cue | Sensor |
|---|---|---|---|
| `ADMIRE` | Speak/make noise at the device | Happy "bark" | Microphone |
| `BRIBE` | Drop a coin into the slot | Gold coin clinking | Infrared sensor |
| `INTIMIDATE` | Shake the box | Angry "bark" | Accelerometer |

## Test Sketches

Each file in `Test Sketches/` isolates a single component so it can be wired
up and verified independently (via the Serial Monitor) before being
integrated into the full `bop_it.ino` build. Recommended bring-up order:

1. `TestAccel.ino`: confirm the accelerometer responds and tune `accelThreshold`.
2. `TestMic.ino`: confirm the mic module responds and tune `micThreshold`.
3. `TestPhoto.ino`: confirm the infrared sensor responds and tune `photoThreshold`.
4. `TestHexDisplay.ino`: confirm the 7-segment display wiring and segment polarity (`HEX_COMMON_ANODE`).
5. `TestMP3.ino`: confirm the DFPlayer Mini plays tracks off the microSD card.

Once each sensor behaves correctly in isolation, the corresponding pins,
thresholds, and logic should match what is already implemented in
`bop_it.ino`.

## Reproducing this project

1. **Hardware**: an Arduino Nano, an MPU6050 accelerometer breakout (I2C),
   a microphone module, an infrared/photo sensor, a 2-digit 7-segment
   display (two 74HC595 shift registers), a DFPlayer Mini MP3 module with a
   microSD card, a push button, and red/green LEDs.
2. **Wiring**: connect everything according to the pin map above.
3. **MP3 tracks**: load the microSD card with numbered tracks matching the
   `playMP3()` calls in `bop_it.ino` (startup, start, success, failure,
   victory, and the three action-instruction sounds).
4. **Bring-up**: flash each sketch in `Test Sketches/` first to confirm
   wiring and calibrate thresholds for your components.
5. **Play**: upload `bop_it.ino`, open the Serial Monitor at 9600 baud to
   observe sensor readings and game state, then press the start button to
   begin.
