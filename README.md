#  STM32 Rocket Flight Computer

Custom **plug-and-play rocket flight computer** designed for telemetry, data logging, and actuator control using the **STM32F303VCT6** microcontroller.

This project integrates an **IMU, barometer, SD card logging, LoRa telemetry, and servo outputs**, making it suitable for **model rockets, avionics development, and embedded systems experimentation**.

---

# Overview

This project implements a **modular rocket flight computer** capable of:

- Measuring **acceleration and angular velocity**
- Estimating **altitude using a barometer**
- Logging flight data to an **SD card**
- Transmitting telemetry via **LoRa radio**
- Controlling **servo actuators**
- Supporting **future pyro channel deployment**

The system is designed to be **plug-and-play**, allowing easy expansion through dedicated headers for additional sensors or payload hardware.

---

# Microcontroller

**MCU:** STM32F303VCT6  
- ARM Cortex-M4  
- Floating Point Unit (FPU)  
- High-speed SPI, UART, I2C peripherals  
- Ideal for real-time embedded systems

Package: **100-pin LQFP**

---

# Hardware Layout

| Component | Interface | Pins |
|-----------|-----------|------|
| **LSM6DSOX IMU** | SPI1 | PA5 / PA6 / PA7, CS = PA10 |
| **BMP388 Barometer** | SPI2 | PB13 / PB14 / PB15, CS = PA8 |
| **SD Card** | SPI3 | PC10 / PC11 / PC12, CS = PD2 |
| **LoRa E32-433T20D** | USART2 | PA2 (TX) / PA3 (RX) |
| **LoRa Control Pins** | GPIO | M0 = PC0, M1 = PC1 |
| **LoRa Status** | GPIO | AUX = PC2 |
| **Servo 1** | TIM3 CH1 PWM | PE2 |
| **Servo 2** | TIM3 CH2 PWM | PE3 |
| **Servo 3** | TIM3 CH3 PWM | PE4 |
| **SWD Debug** | SWD | PA13 / PA14 |
| **Expansion Header** | GPIO / I2C / ADC / UART | PB6 / PB7 / PA0 / PA1 / PD8 / PD9 |
| **HSE Crystal** | Oscillator | PF0 / PF1 |
| **LSE Crystal** | RTC Oscillator | PC14 / PC15 |
| **Pyro Channels** | GPIO (planned) | TBD |

---

# System Features

## Sensor Suite

### LSM6DSOX IMU
Provides:

- 3-axis accelerometer
- 3-axis gyroscope
- High-speed motion sensing for flight detection

Interface: **SPI**

---

### BMP388 Barometer

Provides:

- Pressure measurement
- Altitude estimation
- Vertical velocity estimation

Interface: **SPI**

---

# Data Logging

Flight data is logged to an **SD card via SPI**.

Logged data includes:

- IMU acceleration
- Gyroscope data
- Barometric altitude
- System timestamps
- Telemetry status
- Servo commands

This allows **post-flight analysis and debugging**.

---

# Telemetry

Telemetry is transmitted using the **LoRa E32-433T20D module**.

Transmitted data may include:

- Altitude
- Acceleration
- Orientation
- Flight state
- System health

This enables **real-time flight monitoring from a ground station**.

---

# Servo Control

The flight computer supports **three PWM-controlled servos**.

Driven by **TIM3 hardware PWM channels**.

Possible uses:

- Control surfaces
- Payload deployment
- Experimental flight control
- Throttle or valve control

---

# Pyro Channels (Planned)

Future versions will include **pyrotechnic deployment channels** for:

- Drogue parachute deployment
- Main parachute deployment
- Stage separation

Safety features will include:

- Software arming logic
- Continuity checks
- Redundant deployment logic

---

# Firmware Architecture

Firmware is written in **C/C++ using STM32 HAL** and generated using **STM32CubeMX**.

Example structure:

```
Core/
    main.c
    imu_driver.c
    barometer_driver.c
    lora_driver.c
    sd_logger.c
    servo_control.c
```

Main responsibilities:

- Sensor acquisition
- Flight state estimation
- Telemetry transmission
- Data logging
- Actuator control

---

# Planned Flight Software

A flight **state machine** will control rocket logic:

```
IDLE
ARMED
LAUNCH_DETECTED
ASCENT
APOGEE
DESCENT
LANDED
```

Future features may include:

- Apogee detection
- Sensor fusion
- Kalman filtering
- Fault handling
- Event-based logging

---

# Development Environment

Tools used for development:

- STM32CubeMX
- VS Code
- PlatformIO
- J-Link debugger
- STM32 HAL libraries

---

# Hardware Design

The flight computer will be implemented on a **custom PCB** containing:

- STM32 microcontroller
- SPI sensors
- LoRa radio interface
- SD card slot
- Servo connectors
- Expansion header
- Debug interface

The design is optimized for **compact rocket avionics bays**.

---

# Future Improvements

Planned upgrades:

- GPS integration
- CAN bus support
- RTOS scheduling
- Redundant sensors
- Ground station software
- Advanced telemetry
- Pyrotechnic deployment circuits

---

# Author

**Taylen-Khang Vo**  
Electrical & Computer Engineering  

Focus areas:

- Embedded systems
- Firmware engineering
- Rocket avionics
- Hardware/software co-design

---

# Disclaimer

This project is intended for **educational and experimental purposes only**.

Rocket avionics systems should be designed with **proper safety, redundancy, and testing procedures** before use in real flight environments.
