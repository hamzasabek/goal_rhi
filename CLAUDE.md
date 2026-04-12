# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Purpose

GOAL_RHI is an embedded firmware for a **Rubber Hand Illusion (RHI) neuroscience experiment**. A participant moves their index finger; the BNO085 IMU captures the motion and replays it — with a configurable time delay — onto a rubber/robotic hand driven by a servo. The experiment studies how temporal synchrony affects body ownership perception.

## Build & Flash Commands

This project uses **PlatformIO** targeting a **Teensy 4.1**.

```bash
# Build
pio run

# Build + flash to Teensy
pio run -t upload

# Open serial monitor (115200 baud, with timestamps)
pio device monitor

# Build + flash + monitor in one step
pio run -t upload && pio device monitor

# Clean build artifacts
pio run -t clean
```

There are no automated unit tests — `test/` is reserved for future PlatformIO native tests. Manual testing is done via the serial monitor by following experiment prompts.

## Architecture

### Module Breakdown

All source in `src/`, all headers in `include/`. The split is clean: one `.cpp` per concern.

| Module | Responsibility |
|---|---|
| `Config.h` | All constants and pin assignments — **single source of truth**. Change hardware wiring or experiment parameters here only. |
| `Drivers` | PCA9685 servo driver (via `Wire2`) + vibration motors. `bougerMainCaoutchouc(float angle)` converts a 0–90° angle to inverted PWM pulse and sends it to the servo. |
| `Sensors` | BNO085 IMU (via `Wire`) + potentiometer fallback + button inputs. `lireAngleDoigt()` returns the finger's current angle relative to the calibrated zero. `calibrerCapteur()` sets that zero. |
| `Experiment` | Trial scheduling, the circular delay buffer, and the main trial execution loop (`executerUnEssai`). |
| `Logging` | CSV header and per-trial data output over Serial: `Trial,Delay,Noise,Moves,Time,Response`. |

### Key Data Flow

```
IMU (BNO085 pitch axis)
  → lireAngleDoigt()          [relative angle after calibration]
  → mettreAJourCompteurMouvement()  [hysteresis counter: up+down = 1 move]
  → calculerPositionRetardee()      [circular buffer: replay angle N ms ago]
  → bougerMainCaoutchouc()          [inverted angle → PCA9685 PWM]
```

### Delay Mechanism

`calculerPositionRetardee()` in `Experiment.cpp` implements a **circular ring buffer** (`memoirePositions[60]`). The loop runs every 20 ms; each slot therefore represents 20 ms. A delay of `D` ms reads from `D/20` slots back. Maximum bufferable delay = 60 × 20 ms = **1200 ms**. Exceeding this silently wraps — keep `delaiMs / 20 < tailleBuffer`.

### Sensor Selection

`Config.h` has `const TypeCapteur CAPTEUR_ACTUEL = IMU_BNO085;`. Switching to `POTENTIOMETRE` changes the read path in both `Sensors.cpp` and the threshold constants used for movement detection.

### I2C Bus Assignment

- `Wire` (pins 18/19) → BNO085 IMU
- `Wire2` → PCA9685 servo driver (addr `0x40`)

Both busses are clocked at 100 kHz (`setClock(100000)`).

### Trial Structure

10 trials are constructed as a **2×5 factorial design** (5 delays × vibration ON/OFF), shuffled with `randomSeed(analogRead(A1))`. The delay array in `preparerListeEssais()` currently contains all zeros — populate `d[]` with the desired delay values (e.g. `{0, 10, 50, 150, 300}`) to activate the asynchrony conditions.

### Serial Protocol (operator interface)

- Type `d` + Enter to start a trial after calibration.
- Type `o`/`n` (or press buttons on pins 3/5) to record YES/NO response.
- Lines prefixed `DATA,` are the loggable CSV rows; all other output is operator feedback.

## Active Development Notes (from `docs/CONTEXT.md`)

The planned full design has **two robotic servos** (Servo0 & Servo1) plus a **Servo3** that physically moves the participant's finger (passive condition). The current firmware implements only the single-servo active condition. Labs 00–06 in `docs/LABS.md` outline the incremental build-up toward the full rig.
