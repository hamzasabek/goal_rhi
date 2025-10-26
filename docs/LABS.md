# GOAL_RHI Hands-on Labs

Use this lab sequence to build the GOAL_RHI rig incrementally. Each lab adds a functional slice—start with Lab 00 and keep notes from your experiments. Capture logs, diffs, oscilloscope traces, and photos in `experiments/` to build a reproducible record.

## Lab Map
| Lab | Theme | Core Deliverable |
| --- | ----- | ---------------- |
| 00  | Tooling & safety baseline | Verified PlatformIO build + risk checklist |
| 01  | I²C bring-up | Teensy talks to BNO085 & PCA9685 |
| 02  | IMU neutral + orientation | Streaming quaternion + neutral capture |
| 03  | Servo calibration | Calibrated pulse ranges + safe centering |
| 04  | Active trials pipeline | IMU → virtual servos with programmable delay |
| 05  | Passive trials pipeline | Servo3 motion script + follower servos |
| 06  | Experiment control & logging | Trial scheduler, CSV logs, latency audit |

---

## Lab 00 — Environment & Safety Baseline
**Objectives**
- Stand up the PlatformIO toolchain for Teensy 4.1.
- Document wiring/power safety rules before energising hardware.
**Prerequisites**
- Teensy 4.1 USB connected, PlatformIO extension in VS Code installed.
**Hardware & Prep**
- Keep servos unplugged from horns/loads.
- Multimeter ready; power supply current limit < 1 A for first boot.
**Tasks**
1. Clone/pull repository and open in VS Code.
2. Run `PlatformIO: Build` and confirm the skeleton firmware compiles.
3. Flash the Teensy and open the serial monitor; capture the banner + heartbeat log.
4. Draft a one-page safety checklist (power-up order, emergency stop, neutral posture) in `docs/` or `experiments/`.
**Checkpoint**
- Serial terminal shows `GOAL_RHI v0.2-lab0.2D` heartbeat without runtime faults.
- Safety checklist reviewed with a teammate or mentor.
**Extensions**
- Automate build + monitor via `pio run -t upload && pio device monitor`.

## Lab 01 — I²C Bring-up
**Objectives**
- Verify the Teensy communicates with both I²C peripherals (BNO085 & PCA9685).
- Establish error-handling paths for bus faults.
**Prerequisites**
- Lab 00 complete; wiring installed per `CONTEXT.md` pin map.
**Hardware & Prep**
- Use shortened STEMMA-QT cable; confirm pull-ups (3.3 V) present.
- Power the servo rail with bench supply but keep the OE pin wired low (enabled).
**Tasks**
1. Create `experiments/lab01_i2c_scan/` and copy `src/main.cpp` as baseline.
2. Implement a simple I²C scanner (Wire.begin, iterate 0x03–0x77) that logs detected addresses once per second.
3. Flash and validate that `0x4A` (BNO085) and `0x40` (PCA9685) appear.
4. Unplug each peripheral in turn to confirm the scanner reports the loss and resumes when reconnected.
5. Record findings and bus waveforms (if available) in a short lab note.
**Checkpoint**
- Scanner reliably lists both devices and handles hot-plug gracefully.
**Extensions**
- Add retry/backoff logic for `Wire` errors and log the recovery time.

## Lab 02 — IMU Neutral & Orientation Stream
**Objectives**
- Consume real quaternion/rotation vector reports from the BNO085.
- Implement neutral capture and angle computation along the configured axis.
**Prerequisites**
- Lab 01 complete; `Adafruit_BNO08x` library installed via `lib_deps`.
**Hardware & Prep**
- IMU mounted on finger mock-up; keep finger static during neutral capture.
**Tasks**
1. Fork code into `experiments/lab02_imu/`; enable `imu.begin()` to initialise the BNO085 (set I²C frequency, enable rotation vector report at ≥100 Hz).
2. Implement `ImuDriver::update()` to read events, store quaternion, and timestamps.
3. Add `captureNeutral()` command via serial (`n` key) that stores `_q0`.
4. Compute signed extension angle in degrees using quaternion difference projected onto `_ax/_ay/_az`.
5. Stream angle + raw quaternion at 100 Hz over serial or USB MIDI (choose one).
**Checkpoint**
- Angle readout returns near-zero after neutral capture and tracks finger extension smoothly with minimal drift.
**Extensions**
- Log angle/gyro bias to CSV (`/data`) for offline analysis.

## Lab 03 — Servo Calibration & Safety Limits
**Objectives**
- Drive servos through PCA9685 with controlled pulse widths.
- Characterise safe min/max/neutral microsecond values for each channel.
**Prerequisites**
- Lab 01 complete (PCA detected); servo power rail verified.
**Hardware & Prep**
- Attach servos with load removed; keep mechanical travel unobstructed.
- Use current-limited supply (start 6.0 V, 0.8 A limit).
**Tasks**
1. Create `experiments/lab03_servos/` variant enabling PCA9685 at 50 Hz.
2. Implement `ServoBus::begin()` and `writeUS()` using Adafruit PWM library (convert µs to ticks).
3. Sweep each servo from nominal neutral to tentative min/max in 10° increments; log observed angles and current draw.
4. Update `SERVO*_US_MIN/MAX/NEU` in `config.h` or a per-lab override header.
5. Add software clamps and watchdog to return to neutral if command rate stalls.
**Checkpoint**
- All servos respond smoothly with confirmed safe µs ranges; watchdog tested by halting loop (e.g., `while(true);`).
**Extensions**
- Record servo trajectories with a high-speed camera for later validation.

## Lab 04 — Active Trials Pipeline
**Objectives**
- Map IMU angle to Servo0/Servo1 target positions with configurable delay.
- Implement synchronous (0 ms) and asynchronous (10–300 ms) modes.
**Prerequisites**
- Labs 02 and 03 complete; IMU and servos calibrated.
**Hardware & Prep**
- Participant finger free to move; ensure mechanical stops limit hyperextension.
**Tasks**
1. Build state machine: `IDLE → CALIBRATE_NEUTRAL → ACTIVE_TRIAL`.
2. Add serial command interface: `start`, `delay <ms>`, `mode active`.
3. Implement circular buffer or timestamp queue to schedule Servo1 command after the selected delay, while Servo0 follows in real time.
4. Log IMU angle, Servo0 µs, Servo1 µs, queue depth, and computed latency each cycle.
5. Execute trial script: 5 voluntary extensions with each delay setting; store logs in `experiments/lab04_active/`.
**Checkpoint**
- Verifiable latency (scope or timestamp comparison) matches configured delay within ±2 ms.
**Extensions**
- Add visual indicator (LED) showing when buffer saturates or delay deviates.

## Lab 05 — Passive Trials Pipeline
**Objectives**
- Drive Servo3 with predefined motion curves that move the participant’s finger.
- Make Servo0/Servo1 mirror Servo3 with optional delay offsets.
**Prerequisites**
- Lab 03 servo control stable; mechanical coupling between Servo3 and finger tested manually (unpowered).
**Hardware & Prep**
- Use soft coupler to avoid injury; operator ready on emergency stop.
**Tasks**
1. Author motion profiles (e.g., trapezoidal velocity) in `/data/profiles/*.csv`.
2. Implement loader that plays profiles at 100–200 Hz, interpolating between points.
3. Route Servo3 commands from profile, Servo0/1 commands from same buffer with applied delay.
4. Record IMU data concurrently for quality assurance (even though not used to drive motion).
5. Run passive trial sequence (e.g., sync, 50 ms, 150 ms) and log actuator currents/temperatures.
**Checkpoint**
- Servo3 executes profile without overshoot; Servo0/1 trace matches expected delay when plotted.
**Extensions**
- Add torque/current sensing (Hall sensor) to detect excessive load and halt movement.

## Lab 06 — Experiment Control & Logging
**Objectives**
- Build a configurable trial runner with precise timing guarantees.
- Persist structured logs suitable for statistical analysis.
**Prerequisites**
- Labs 04 and 05 functional; host PC ready to capture data.
**Hardware & Prep**
- USB serial link stable; optional BLE or network logging disabled for latency tests.
**Tasks**
1. Design a `TrialConfig` struct (mode, delay, repetitions, profile path) and serial protocol (`JSON` or simple key-value).
2. Implement scheduler that executes a trial list, enforces rest periods, and signals operator prompts over serial (`READY`, `MOVE`, `REST`).
3. Timestamp all events with both firmware `millis()` and microsecond-resolution cycle timer; include queue backlog metrics.
4. Stream logs in CSV/CBOR to host; create Python notebook in `experiments/lab06_logging/` that parses and plots latency histograms.
5. Inject fault scenarios (disconnect IMU, block servo) and verify that the system logs the fault, halts motion, and requires manual reset.
**Checkpoint**
- Full trial session (active + passive) executes with synchronized logs and documented latency < 5 ms jitter (excluding intentional delay).
**Extensions**
- Integrate external trigger (GPIO) to sync with EEG/EMG acquisition systems.

---

## Next Steps
- Formalise lessons learned from each lab into the main firmware (`src/`), merging only when tests pass.
- Maintain a running risk register (hardware + software) and revisit before human-in-the-loop sessions.
- Share lab artefacts (plots, configs, notebooks) with collaborators through version control or shared drive.
