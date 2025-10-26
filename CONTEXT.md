# GOAL_RHI — Project Context for Coding Assistant (Codex/Copilot)

## Goal
Build a real-time rig that maps index-finger movement (IMU on the phalanx between PIP and DIP) to **two robotic servos** (rubber hands), with precise **synchrony/asynchrony** control. In **active** trials, the participant voluntarily extends the index (palm on table) and the two servos move either **synchronously** or with a **delay** (10, 20, 50, 150, 300 ms). In **passive** trials, a **third servo** moves the real finger while the two robotic servos follow (sync or delayed). Low latency, safe limits, and robust logging are required.

---

## Experimental Scenarios

### Active condition
- IMU captures **extension** of the participant’s index finger.
- Servo0 & Servo1 reproduce the motion.
- Modes: **sync** (0 ms) or **asynchrony** = {10, 20, 50, 150, 300} ms where Servo1 is delayed.

### Passive condition
- **Servo3** moves the real finger (predefined motion profile).
- Servo0 & Servo1 follow Servo3 (sync or same set of delays).
- No IMU mapping to servos in this condition (IMU may still log for quality checks).

---

## Hardware (Bill of Materials – current working set)
- **MCU**: Teensy 4.1 (Arduino framework via PlatformIO).
- **IMU**: Adafruit **BNO085** (9-DoF, SH-2 fusion), connected via **STEMMA QT (I²C)**.
- **Servo driver**: **PCA9685** (Adafruit), used in **signal-only** mode (no high-current power through the board).
- **Servos**:
  - Servo0 & Servo1: robotic hands (MCP/PIP/DIP allocation will be tuned later).
  - Servo3: real-finger actuator (passive condition).
  - (Models you’ve used: e.g., RJX Hobby FS0391HV or similar; start at 6.0 V for safety, adjust per spec).
- **Power**:
  - Separate **servo power rail** (6.0–7.4 V depending on servo model), with **large bulk caps** (1000–2200 µF) + local 100 nF.
  - **Star ground**: Teensy GND, PCA GND, IMU GND, and servo rail GND meet at a **single star point (GND★)**.
  - Buck converters (e.g., LM2596, 300 W DC/DC) for servo rail; **do not** feed PCA V+ with >6 V.
- **Wiring**:
  - Teensy I²C0: SDA=18, SCL=19 to PCA9685 & BNO085 (via STEMMA).
  - PCA9685 default I²C addr **0x40**; BNO085 default **0x4A** (0x4B if DI high).
  - PCA’s **OE → GND** (enabled). PCA’s **VCC 3.3 V** from Teensy; **V+ (servo power)** from servo rail.
- **Mechanical**:
  - Mount IMU firmly between PIP and DIP, cable strain relieved.
  - Remove horn/links during first powered tests (safety).

---

## Software Stack

- **Tooling**: VS Code + **PlatformIO** (board `teensy41`, framework `arduino`).
- **Key libraries**:
  - `Adafruit_BNO08x` (install via GitHub if registry fails).
  - `Adafruit PWM Servo Driver Library` (PCA9685).
  - `Adafruit BusIO`.
- **platformio.ini (minimal)**:
  ```ini
  [env:teensy41]
  platform      = teensy
  board         = teensy41
  framework     = arduino
  monitor_speed = 115200
  monitor_filters = time
  build_flags   = -DUSB_SERIAL
  lib_deps =
    adafruit/Adafruit BusIO
    adafruit/Adafruit PWM Servo Driver Library
    https://github.com/adafruit/Adafruit_BNO08x
