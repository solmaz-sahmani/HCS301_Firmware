# HCS301 Firmware

A multi-firmware embedded system for a 433 MHz rolling-code remote control based on the Microchip HCS301 protocol.

The project implements a shared, hardware-independent firmware architecture with separate transmitter and receiver applications:

* **ESP32** — transmitter
* **STM32** — receiver
* **FS1000A** — 433 MHz RF module on both sides

The project is developed using **C**, **CMake**, and **ESP-IDF**. Shared firmware modules are designed to remain independent of the target MCU whenever possible.

---

## System Overview

```text
                         HCS301 REMOTE SYSTEM

        TRANSMITTER                                 RECEIVER
          ESP32                                       STM32
             │                                          │
             ▼                                          ▼
       Button GPIO                                 RF RX capture
             │                                          │
             ▼                                          ▼
     button_manager                                  rf_driver
             │                                          │
             ▼                                          ▼
     hcs301_protocol                              hcs301_decoder
             │                                          │
             ▼                                          ▼
       KeeLoq encrypt                             66-bit frame
             │                                          │
             ▼                                          ▼
          66 bits                                hcs301_protocol
             │                                          │
             ▼                                          ▼
     hcs301_encoder                               KeeLoq decrypt
             │                                          │
             ▼                                          ▼
        RF pulses                                 HCS301 frame
             │                                          │
             ▼                                          ▼
        rf_driver                                    Verify
             │                                          │
             ▼                                          ▼
          FS1000A                                 remote_storage
             │                                          │
             │                433 MHz RF                 │
             └──────────────────────────────────────────┘
                                   │
                                   ▼
                              Application
                                   │
                                   ▼
                             led_indicator
                                   │
                                   ▼
                                  LEDs
```

---

## Main Features

### HCS301

* HCS301 logical frame encoding and decoding
* 66-bit HCS301 frame handling
* KeeLoq encryption/decryption
* Fixed and encrypted field handling
* Serial number handling
* Discrimination handling
* Button status handling
* Counter handling
* Overflow handling

### RF

* HCS301 pulse encoder
* HCS301 pulse decoder
* RF transport abstraction
* FS1000A transmitter integration
* FS1000A receiver integration
* Hardware-independent RF data structures

### Remote Management

* Remote pairing
* Remote deletion
* Static remote storage
* Memory pool for remote records
* No dynamic allocation
* Counter storage for paired remotes

### Application Services

* Button management
* LED indication
* Timer management
* Device configuration
* CLI configuration
* Error/status handling
* Power-management abstraction

### Architecture

* C firmware
* Opaque module types
* Static allocation
* No `malloc()` / `free()`
* Separation of concerns
* Hardware-independent shared drivers
* Separate firmware targets
* Unit/integration testing
* CMake-based builds

---

# Architecture

The project is organized into four major layers.

```text
┌─────────────────────────────────────────────────────────────┐
│                     APPLICATION LAYER                       │
│                                                             │
│   Transmit flow / Receive flow / Pairing / Delete / LEDs    │
└──────────────────────────────┬──────────────────────────────┘
                               │
┌──────────────────────────────▼──────────────────────────────┐
│                  MANAGERS / SERVICES                        │
│                                                             │
│ button_manager   pairing_manager   delete_manager           │
│ remote_storage   led_indicator    timer_manager             │
│ device_config    cli_service      power_manager             │
└──────────────────────────────┬──────────────────────────────┘
                               │
┌──────────────────────────────▼──────────────────────────────┐
│                  HCS301 / RF LAYER                          │
│                                                             │
│ hcs301_protocol                                             │
│ hcs301_encoder       hcs301_decoder                         │
│ keeloq_sw             rf_driver                             │
└──────────────────────────────┬──────────────────────────────┘
                               │
┌──────────────────────────────▼──────────────────────────────┐
│                       HARDWARE LAYER                        │
│                                                             │
│  GPIO / TIMER / UART / INTERRUPT / RF / FLASH / POWER       │
│                                                             │
│         ESP32                         STM32                 │
└─────────────────────────────────────────────────────────────┘
```

The shared firmware logic should not depend directly on MCU registers.

Hardware-specific code belongs inside the corresponding firmware target.

---

# Transmitter Data Path

The ESP32 transmitter follows this path:

```text
Button GPIO
     │
     ▼
button_input
     │
     ▼
button_manager
     │
     ▼
HCS301 frame creation
     │
     ▼
hcs301_protocol
     │
     ▼
KeeLoq encryption
     │
     ▼
66 logical bits
     │
     ▼
hcs301_encoder
     │
     ▼
RF pulse train
     │
     ▼
rf_driver
     │
     ▼
FS1000A
     │
     ▼
433 MHz
```

The physical GPIO layer is kept outside `button_manager`.

---

# Receiver Data Path

The STM32 receiver follows this path:

```text
433 MHz
   │
   ▼
FS1000A
   │
   ▼
STM32 GPIO / Timer Capture
   │
   ▼
rf_driver
   │
   ▼
rf_pulse_t[]
   │
   ▼
hcs301_decoder
   │
   ▼
66 logical bits
   │
   ▼
hcs301_protocol
   │
   ▼
KeeLoq decryption
   │
   ▼
HCS301 frame
   │
   ▼
hcs301_protocol_verify()
   │
   ▼
remote_storage
   │
   ▼
Application
   │
   ▼
led_indicator
   │
   ▼
STM32 GPIO
```

---

# HCS301 Data Processing

The HCS301 processing chain is intentionally divided into separate responsibilities.

## `hcs301_protocol`

Responsible for logical HCS301 data:

```text
HCS301 frame
      ↕
66 logical bits
```

Responsibilities:

* Encode
* Decode
* Verify
* Field extraction
* KeeLoq integration

It does **not** handle RF pulse timing.

---

## `hcs301_encoder`

Converts logical HCS301 bits into physical pulse timing:

```text
66 bits
   │
   ▼
hcs301_encoder
   │
   ▼
RF pulse train
```

The encoder handles:

* Preamble
* Header
* Data-bit timing
* Guard period
* Pulse duration

---

## `hcs301_decoder`

Performs the reverse operation:

```text
RF pulse train
      │
      ▼
hcs301_decoder
      │
      ▼
66 bits
```

The decoder does not:

* decrypt KeeLoq
* validate a remote
* manage pairing
* update counters
* control LEDs

Those responsibilities belong to other modules.

---

# Encoder / Decoder Round Trip

The software representation can be tested without RF hardware:

```text
        66 bits
           │
           ▼
   hcs301_encoder
           │
           ▼
      RF pulses
           │
           ▼
   hcs301_decoder
           │
           ▼
        66 bits
           │
           ▼
       Compare
```

The expected result is:

```text
Original bits == Decoded bits
```

This verifies the software pulse encoding/decoding layer before testing the physical FS1000A link.

---

# KeeLoq

The project contains a software implementation of the KeeLoq block cipher.

```text
Block size : 32-bit
Key size   : 64-bit
Rounds     : 528
```

Test vectors use non-production test keys and values.

Example:

Plaintext  : test vector
Key        : non-production test key
Ciphertext : expected test-vector result

---

# Remote Storage

Paired remotes are stored using static allocation.

The storage architecture uses a fixed memory pool rather than dynamic allocation.

```text
remote_storage
      │
      ▼
┌──────────────────────┐
│ Remote Record #0     │
├──────────────────────┤
│ Remote Record #1     │
├──────────────────────┤
│ Remote Record #2     │
├──────────────────────┤
│ ...                  │
├──────────────────────┤
│ Remote Record #N     │
└──────────────────────┘
```

Typical remote information includes:

```text
serial
counter
```

The architecture is designed so that the number of remote records can be managed by the configured static pool size.

No `malloc()` or `free()` is used.

---

# Pairing

Pairing is implemented as a state machine.

```text
PAIRING_IDLE
     │
     ▼
PAIRING_WAITING
     │
     │ valid remote frame
     ▼
PAIRING_SUCCESS
     │
     ▼
remote_storage
     │
     ▼
LED indication
```

After successful pairing, the receiver can indicate success by blinking all four LEDs for the configured period.

---

# Delete

Remote deletion is handled separately from pairing.

```text
DELETE_IDLE
     │
     ▼
DELETE_WAITING
     │
     │ remote selected
     ▼
DELETE_SUCCESS
     │
     ▼
remote_storage
```

This keeps deletion logic independent from RF transport and physical GPIO handling.

---

# Device Configuration

`device_config` manages HCS301 device parameters.

Current configuration includes:

```text
64-bit encryption key
28-bit serial number
10-bit discrimination value
```

Example:

```text
KEY    = 0x0123456789ABCDEF
SERIAL = 0x02AF379
DISC   = 0x379
```

Configuration validation is performed by the software layer.

Persistent storage is a hardware-dependent feature and will be implemented through the appropriate MCU storage backend.

---

# Button Manager

The logical button manager represents:

```text
BUTTON_PAIR
BUTTON_DELETE
BUTTON_1
BUTTON_2
BUTTON_3
BUTTON_4
```

It does not directly access GPIO.

The architecture is:

```text
Physical GPIO
     │
     ▼
button_input
     │
     ▼
button_manager
     │
     ▼
Logical button state
```

This allows the same logical button manager to be reused by different MCU targets.

---

# LED Indicator

`led_indicator` manages the logical state of four LEDs.

```text
led_indicator
      │
      ▼
Logical LED state
      │
      ▼
LED HAL
      │
      ▼
MCU GPIO
```

The shared module does not directly access MCU registers.

---

# Timer Manager

`timer_manager` provides hardware-independent timing logic.

The manager can be updated with elapsed time:

```c
timer_manager_update(manager, elapsed_ms);
```

This allows timing-related logic to be tested independently of a specific MCU timer peripheral.

Hardware timer implementation belongs to the firmware target.

---

# RF Driver

`rf_driver` provides the shared RF transport interface.

Its purpose is to separate:

```text
HCS301 software logic
```

from:

```text
RF hardware implementation
```

The logical architecture is:

```text
HCS301
   │
   ▼
rf_driver
   │
   ▼
RF HAL
   │
   ▼
FS1000A / MCU hardware
```

The shared driver must remain independent of ESP32 and STM32-specific APIs.

---

# Hardware Targets

## ESP32 Transmitter

The ESP32 firmware is responsible for:

```text
Button input
     ↓
HCS301 frame generation
     ↓
HCS301 encoding
     ↓
RF transmission
     ↓
FS1000A TX
```

Current transmitter hardware concept:

```text
Button A → GPIO 4
Button B → GPIO 5
Button C → GPIO 6
Button D → GPIO 7

FS1000A DATA → GPIO 17
```

The exact hardware configuration is kept inside the ESP32 hardware layer.

The ESP32 firmware is built with:

```text
ESP-IDF
```

---

# STM32 Receiver

The STM32 firmware is responsible for:

```text
FS1000A RX
     ↓
Pulse capture
     ↓
HCS301 decoder
     ↓
Protocol decoding
     ↓
Remote validation
     ↓
Application
     ↓
LED output
```

The STM32 firmware is built using:

```text
CMake
ARM GNU Toolchain
```

STM32-specific GPIO, timer, interrupt and other peripheral code remains inside the STM32 hardware target.

---

# Shared Drivers

The project is designed as a multi-firmware system.

Shared software is reused by both targets where possible:

```text
                         Shared Drivers
                              │
              ┌───────────────┴───────────────┐
              │                               │
              ▼                               ▼
            ESP32                           STM32
          ESP-IDF                         CMake/ARM GCC
              │                               │
              ▼                               ▼
        Hardware Layer                  Hardware Layer
```

The goal is:

```text
Same protocol logic
Same encryption logic
Same encoder/decoder logic
Same managers where possible
Different hardware backends
```

---

# Project Structure

Current high-level structure:

```text
HCS301_Firmware/
│
├── CMakeLists.txt
├── README.md
│
├── drivers/
│   ├── button_manager/
│   ├── cli_service/
│   ├── delete_manager/
│   ├── device_config/
│   ├── err_core/
│   ├── hcs301_encoder/
│   ├── hcs301_decoder/
│   ├── hcs301_protocol/
│   ├── keeloq_sw/
│   ├── led_indicator/
│   ├── pairing_manager/
│   ├── power_manager/
│   ├── remote_storage/
│   ├── rf_driver/
│   └── timer_manager/
│
├── hardware/
│   ├── ESP32/
│   │   └── ...
│   │
│   └── STM32/
│       └── ...
│
└── tests/
    └── ...
```

The exact hardware-specific source structure may differ between the ESP32 and STM32 targets.

---

# CMake Architecture

The shared drivers are implemented as reusable components.

For ESP-IDF, driver components use:

```cmake
idf_component_register(...)
```

The root ESP-IDF project uses:

```cmake
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(hcs301_firmware)
```

The shared software modules should not depend on ESP-IDF APIs unless the module is explicitly hardware-specific.

The STM32 target uses its own CMake build and ARM GCC toolchain.

---

# Testing Strategy

Testing is divided into several levels.

## Unit Tests

Individual modules are tested independently.

Examples:

```text
keeloq_sw
remote_storage
timer_manager
device_config
hcs301_protocol
```

---

## Software Integration Tests

Multiple software modules are tested together.

Examples:

```text
hcs301_encoder
       ↓
hcs301_decoder
```

and:

```text
hcs301_protocol
       ↓
keeloq_sw
       ↓
hcs301_protocol
```

---

## Hardware Integration Tests

The final system is tested using real hardware:

```text
ESP32
  ↓
FS1000A TX
  )))))) 433 MHz
FS1000A RX
  ↓
STM32F030F4P6
  ↓
LED
```

This level verifies the actual RF, timing, GPIO and MCU integration.

---

# Design Principles

## No Dynamic Allocation

The firmware does not use:

```c
malloc()
free()
```

Static allocation is preferred for managers and storage.

Variable remote records are handled using a statically allocated memory pool.

---

## Opaque Types

Internal manager structures are hidden from module users.

Example:

```c
typedef struct pairing_manager pairing_manager_t;
```

The user of the module interacts through its public API rather than accessing internal fields.

---

## Separation of Concerns

Each module has a clearly defined responsibility.

For example:

```text
hcs301_decoder
    ↓
Pulse → Bits

hcs301_protocol
    ↓
Bits → HCS301 Frame

keeloq_sw
    ↓
Encryption / Decryption

remote_storage
    ↓
Remote records

pairing_manager
    ↓
Pairing state machine

rf_driver
    ↓
RF transport
```

---

## Hardware Independence

Shared software logic does not directly access:

```text
GPIO registers
Timer registers
UART registers
MCU-specific interrupts
```

Hardware-specific functionality belongs in the appropriate target layer.

---

## Explicit Error Handling

Modules return:

```c
status_t
```

instead of relying on exceptions or hidden global error states.

---

# Current Development Status

```text
Core Software
    │
    ├── Error handling              ✅
    ├── Remote storage              ✅
    ├── Button management           ✅
    ├── LED management              ✅
    ├── Timer management            ✅
    ├── KeeLoq                      ✅
    ├── HCS301 protocol             ✅
    ├── Pairing                     ✅
    ├── Delete                      ✅
    ├── Device configuration        ✅
    ├── CLI                         ✅
    ├── Power abstraction           ✅
    ├── RF abstraction              ✅
    ├── HCS301 pulse encoder        ✅
    └── HCS301 pulse decoder        🟡
    
Software Integration
    │
    ├── Encoder → Decoder test      ⏳
    ├── Protocol round-trip         ⏳
    └── RF driver integration       ⏳

ESP32 Transmitter
    │
    ├── ESP-IDF application          🟡
    ├── Button GPIO                  🟡
    ├── RF GPIO                      🟡
    └── FS1000A TX                   ⏳

STM32 Receiver
    │
    ├── STM32 firmware               🟡
    ├── GPIO                         🟡
    ├── Timer capture                ⏳
    ├── FS1000A RX                   ⏳
    └── Real RF reception            ⏳

System Integration
    │
    ├── ESP32 → FS1000A              ⏳
    ├── FS1000A → STM32              ⏳
    ├── Pairing hardware test        ⏳
    ├── Delete hardware test         ⏳
    └── Full remote-control test     ⏳
```

---

# Development Roadmap

```text
                    HCS301 FIRMWARE
                           │
                           ▼
                  Shared Core Software
                           │
                           ├── Protocol
                           ├── KeeLoq
                           ├── Storage
                           ├── Managers
                           └── RF abstraction
                           │
                           ▼
                 Encoder / Decoder Layer
                           │
                           ├── Encoder
                           └── Decoder
                           │
                           ▼
                 Software Integration
                           │
                           ▼
              ┌────────────┴────────────┐
              ▼                         ▼
         ESP32 TX                   STM32 RX
              │                         │
              ▼                         ▼
         FS1000A TX                 FS1000A RX
              │                         │
              └───────────┬─────────────┘
                          ▼
                   433 MHz RF Link
                          │
                          ▼
                  System Integration
```

---

# Current Goal

The immediate development goal is to complete the software RF path before relying on physical RF hardware:

```text
HCS301 bits
     ↓
hcs301_encoder
     ↓
RF pulse array
     ↓
hcs301_decoder
     ↓
HCS301 bits
```

After this round-trip is verified, the next stage is integration with the real RF driver and STM32 timer/GPIO capture.

---

# License

See `LICENSE` for project licensing information.
