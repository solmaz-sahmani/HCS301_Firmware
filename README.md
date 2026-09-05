# HCS301 Firmware MVP

Firmware architecture for a 433 MHz remote-control system based on the Microchip HCS301 rolling-code encoder.

The project is currently implemented as a **hardware-independent software MVP** because the target MCU and RF hardware have not been selected yet.

The architecture is designed so that the application and protocol logic can remain unchanged when the final MCU and hardware are introduced.

---

## Features

The MVP architecture supports:

* HCS301 logical frame encoding and decoding
* Software KeeLoq encryption and decryption
* Remote pairing
* Remote deletion
* Static remote storage
* Four button states
* Four LED states
* Timer management
* Device configuration
* Serial/CLI configuration
* Power-management abstraction
* RF driver abstraction
* Error/status management
* Integration testing without hardware
* Static allocation without `malloc()` / `free()`

---

## Project Status

The current software integration test passes all tests:

```text
Passed: 49
Failed: 0
RESULT: PASS
```

The project is therefore considered **software-MVP complete**.

Real RF transmission, GPIO, interrupts, timers and low-power hardware operation are intentionally postponed until the target MCU and hardware are selected.

---

## Architecture

```text
                         ┌──────────────────┐
                         │      app         │
                         │     main.c       │
                         └────────┬─────────┘
                                  │
                                  ▼
                         Application Logic
                                  │
             ┌────────────────────┼────────────────────┐
             │                    │                    │
             ▼                    ▼                    ▼
      pairing_manager      delete_manager        cli_service
             │                    │                    │
             └──────────┬─────────┘                    ▼
                        ▼                       device_config
                 remote_storage
                       



                HCS301 Data Path

             RF Driver / Hardware
                      │
                      ▼
                HCS301 Frame
                      │
                      ▼
             hcs301_protocol
                      │
                      ▼
                  keeloq_sw
                      │
                      ▼
              Decoded HCS301 data
                      │
                      ▼
               Application Logic
```

---

## Directory Structure

```text
HCS301_Firmware/
│
├── CMakeLists.txt
├── README.md
│
├── app/
│   ├── CMakeLists.txt
│   └── main.c
│
├── libs/
│   │
│   ├── err_core/
│   │   ├── CMakeLists.txt
│   │   ├── err_core.h
│   │   └── err_core.c
│   │
│   ├── remote_storage/
│   │   ├── CMakeLists.txt
│   │   ├── remote_storage.h
│   │   └── remote_storage.c
│   │
│   ├── button_manager/
│   │   ├── CMakeLists.txt
│   │   ├── button_manager.h
│   │   └── button_manager.c
│   │
│   ├── led_indicator/
│   │   ├── CMakeLists.txt
│   │   ├── led_indicator.h
│   │   └── led_indicator.c
│   │
│   ├── rf_driver/
│   │   ├── CMakeLists.txt
│   │   ├── rf_driver.h
│   │   └── rf_driver.c
│   │
│   ├── keeloq_sw/
│   │   ├── CMakeLists.txt
│   │   ├── keeloq_sw.h
│   │   └── keeloq_sw.c
│   │
│   ├── timer_manager/
│   │   ├── CMakeLists.txt
│   │   ├── timer_manager.h
│   │   └── timer_manager.c
│   │
│   ├── pairing_manager/
│   │   ├── CMakeLists.txt
│   │   ├── pairing_manager.h
│   │   └── pairing_manager.c
│   │
│   ├── delete_manager/
│   │   ├── CMakeLists.txt
│   │   ├── delete_manager.h
│   │   └── delete_manager.c
│   │
│   ├── cli_service/
│   │   ├── CMakeLists.txt
│   │   ├── cli_service.h
│   │   └── cli_service.c
│   │
│   ├── power_manager/
│   │   ├── CMakeLists.txt
│   │   ├── power_manager.h
│   │   └── power_manager.c
│   │
│   ├── device_config/
│   │   ├── CMakeLists.txt
│   │   ├── device_config.h
│   │   └── device_config.c
│   │
│   └── hcs301_protocol/
│       ├── CMakeLists.txt
│       ├── hcs301_protocol.h
│       └── hcs301_protocol.c
│
└── tests/
    ├── CMakeLists.txt
    └── tests.c
```

---

## Module Responsibilities

### `err_core`

Provides common project status codes.

Examples:

```text
STATUS_OK
STATUS_ERROR
STATUS_INVALID_ARG
STATUS_TIMEOUT
STATUS_NOT_FOUND
STATUS_FULL
STATUS_ALREADY_EXISTS
STATUS_NOT_INITIALIZED
STATUS_INVALID_FRAME
STATUS_CRC_ERROR
```

All modules use `status_t` instead of exceptions.

---

### `remote_storage`

Stores paired remotes using a statically allocated pool.

Current capacity:

```text
REMOTE_STORAGE_MAX_REMOTES = 8
```

No dynamic allocation is used.

Each remote contains:

```text
serial
counter
```

---

### `button_manager`

Manages logical button states.

Buttons:

```text
BUTTON_PAIR
BUTTON_DELETE
BUTTON_1
BUTTON_2
BUTTON_3
BUTTON_4
```

Physical GPIO handling is intentionally outside this module.

---

### `led_indicator`

Manages the logical state of four LEDs.

The module does not directly access MCU GPIO.

Physical GPIO control will be added through a hardware layer later.

---

### `timer_manager`

Provides hardware-independent timers.

The application updates timers using:

```c
timer_manager_update(manager, elapsed_ms);
```

This allows the timer logic to be tested on a PC without a hardware timer.

---

### `keeloq_sw`

Software implementation of the KeeLoq block cipher.

Properties:

```text
Block size: 32-bit
Key size:   64-bit
Rounds:     528
```

Known test vector used by the project:

```text
Plaintext  : 0x12345678
Key        : 0x0123456789ABCDEF
Ciphertext : 0xB909A03B
```

The implementation has been verified using encryption/decryption round-trip tests and known vectors.

---

### `hcs301_protocol`

Handles the logical HCS301 frame.

Current logical frame:

```text
Encrypted portion : 32 bits
Fixed portion     : 34 bits
Total              : 66 bits
```

The module is responsible for:

```text
Encode
Decode
Verify
Convert frame to remote information
```

It does not handle physical RF pulse timing.

---

### `pairing_manager`

Controls the pairing state machine.

States:

```text
PAIRING_IDLE
PAIRING_WAITING
PAIRING_SUCCESS
```

A successful pairing stores the remote in `remote_storage`.

After successful pairing, the four LEDs blink for five seconds.

---

### `delete_manager`

Controls the remote deletion flow.

States:

```text
DELETE_IDLE
DELETE_WAITING
DELETE_SUCCESS
```

The manager removes the selected serial number from `remote_storage`.

---

### `device_config`

Stores:

```text
64-bit encryption key
28-bit device serial
10-bit discrimination
```

The module validates the allowed field sizes.

Configuration storage is currently RAM-based.

Persistent Flash/EEPROM storage will be added after the target MCU is selected.

---

### `cli_service`

Provides a simple PC-based command interface.

Current commands:

```text
SET KEY <64-bit-hex>
GET KEY

SET SERIAL <28-bit-hex>
GET SERIAL

SET DISC <10-bit-hex>
GET DISC

HELP
EXIT
```

Current transport:

```text
stdin / stdout
```

When hardware is available, the transport can be replaced by UART without changing `device_config`.

---

### `power_manager`

Provides a hardware-independent power-management interface.

Current interface:

```text
power_manager
      ↓
power_hal_t
```

The PC implementation uses mock callbacks.

A future MCU implementation may connect these callbacks to the MCU low-power functionality.

---

### `rf_driver`

Provides the RF transport abstraction.

Current implementation is a PC loopback mock:

```text
transmit
   ↓
internal buffer
   ↓
receive
```

No physical RF signal is generated.

When the RF hardware is available, this layer will connect to the actual transmitter/receiver hardware.

---

## Real vs Mock

### Real software logic

```text
err_core
remote_storage
button_manager
led_indicator
timer_manager
keeloq_sw
hcs301_protocol
pairing_manager
delete_manager
device_config
```

These modules are intended to remain independent of the selected MCU.

### Hardware-independent abstractions

```text
rf_driver
power_manager
cli_service
```

Their APIs and software logic exist, but their physical backends are currently mocked.

### Current mocks

```text
RF transmission/receive
Power sleep/wake
CLI transport
```

---

## What Changes When an MCU Is Selected?

The main application and protocol logic should remain unchanged.

The following hardware-dependent areas will be implemented:

```text
GPIO
UART
Timers
Interrupts
RF transmitter
RF receiver
Low-power mode
Non-volatile storage
```

The future architecture will look like:

```text
Application
     │
     ▼
Managers / Services
     │
     ▼
Hardware Abstraction Layer
     │
     ▼
MCU peripherals
```

For example:

```text
pairing_manager
       │
       ▼
button_manager
       │
       ▼
GPIO HAL
       │
       ▼
MCU GPIO
```

and:

```text
hcs301_protocol
       │
       ▼
RF driver
       │
       ▼
RF HAL
       │
       ▼
Timer / GPIO / Interrupt
       │
       ▼
433 MHz hardware
```

---

## Intentionally Not Implemented

The following features are intentionally outside the current MVP:

### Physical RF waveform decoding

The project currently does not decode:

```text
433 MHz RF pulses
Preamble
Sync/header timing
PWM timing
Interrupt capture
Pulse timing
```

These require the final RF hardware and MCU.

### Rolling-code counter validation

The project stores the counter but does not yet implement complete replay/freshness validation.

A future receiver should compare the received counter with the stored counter.

### Button consistency validation

The encrypted and fixed button fields are not yet fully cross-checked.

### HCS301 seed mode

The special seed/programming behavior is not implemented in the MVP.

### Persistent storage

Configuration and remote records currently live in RAM.

No EEPROM or MCU Flash backend is implemented yet.

### Button debouncing

The current button manager stores logical state only.

Debouncing and edge/long-press detection will be added at the hardware/application layer when needed.

### Real low-power mode

The power manager API exists, but actual MCU sleep functionality is not implemented.

---

## Build

Clean build:

```powershell
Remove-Item -Recurse -Force build
cmake -S . -B build -G Ninja
cmake --build build
```

---

## Run Integration Test

```powershell
.\build\tests\hcs301_integration_test.exe
```

Expected result:

```text
Passed: 49
Failed: 0
RESULT: PASS
```

---

## Run Firmware Application

```powershell
.\build\app\hcs301_firmware.exe
```

The application starts the system and then waits for CLI commands.

Example:

```text
HCS301 Firmware MVP
===================
System Init : OK

> SET KEY 0123456789ABCDEF
OK

> SET SERIAL 01234567
OK

> SET DISC 155
OK

> GET KEY
KEY: 0x0123456789ABCDEF

> GET SERIAL
SERIAL: 0x01234567

> GET DISC
DISC: 0x155

> EXIT
OK
```

---

## Design Principles

The project follows these principles:

### No dynamic allocation

The firmware does not use:

```c
malloc()
free()
```

Managers and storage use static allocation.

### Opaque types

Manager internals are hidden from users of the module.

Example:

```c
typedef struct pairing_manager pairing_manager_t;
```

### Separation of concerns

Each module has one primary responsibility.

### Hardware independence

Application logic does not depend directly on MCU registers.

### Explicit error handling

Functions return:

```c
status_t
```

instead of using exceptions or hidden global error flows.

### Testability

Modules can be tested on a PC before hardware is available.

### Dependency direction

High-level modules depend on lower-level services, not the reverse.

---

## Current Development Roadmap

```text
Software MVP
    │
    ├── Error handling             ✅
    ├── Remote storage             ✅
    ├── Button management          ✅
    ├── LED management             ✅
    ├── Timer management           ✅
    ├── KeeLoq                     ✅
    ├── HCS301 protocol            ✅
    ├── Pairing                    ✅
    ├── Delete                     ✅
    ├── Device configuration       ✅
    ├── CLI                        ✅
    ├── Power abstraction          ✅
    ├── RF abstraction             ✅
    └── Integration tests          ✅ 49/49
       
Hardware Integration
    │
    ├── Select MCU                 ⏳
    ├── GPIO HAL                  ⏳
    ├── UART HAL                  ⏳
    ├── Timer/Interrupt HAL       ⏳
    ├── RF hardware driver        ⏳
    ├── HCS301 waveform decoder   ⏳
    ├── Flash/EEPROM storage      ⏳
    └── Low-power implementation  ⏳
```

---

## Summary

The current project is a hardware-independent HCS301 firmware MVP.

The core protocol and application logic are implemented and tested independently from the MCU.

The current integration test result is:

```text
49 tests passed
0 tests failed
```

The next major development phase begins only after the target MCU and RF hardware are selected.
