# RSL10 SysTick Timer + Timestamp System

This project implements a real-time timestamp and button event counter system on the **ON Semiconductor RSL10 (ARM Cortex-M3)** using the **SysTick Timer** for millisecond precision. It replaces the RTC module (which only triggers wakeup events in sleep mode) with SysTick for continuous time tracking and interrupt-based event handling.

## 📋 Overview

* Generates **timestamps** every 1 s (format: `HH:MM:SS`)
* Counts button presses within a **10-second window**
* Toggles LED blinking with each button press
* Includes **hybrid debounce** (hardware + software flag)
* Runs in **low-power sleep mode** between interrupts
* Uses **SEGGER RTT** for real-time debug output (no UART required)

Example RTT output:

```
[TIMESTAMP] 00:55:45 - LED OFF  
[CONTADOR] Button: 15 times in last 10 sec  

[GPIO] Button pressed! Total: 16 (timestamp: 3348 sec)  
[GPIO] Button pressed! Total: 17 (timestamp: 3348 sec)  
[GPIO] Button pressed! Total: 18 (timestamp: 3349 sec)  
[TIMESTAMP] 00:55:50 - LED ON  
[CONTADOR] Button: 18 times in last 10 sec  
```

## 🛠️ Hardware

* RSL10 EVB (Evaluation Board)
* ON Semiconductor RSL10 SoC
* Pushbutton **SW2** → `DIO5` (input)
* LED → `DIO6` (output)
* J-Link debugger

## 💻 Software

* **RSL10 SDK** ≥ 3.9.1182
* **onsemi IDE** (Eclipse-based)
* **ARM GCC Toolchain**
* **SEGGER RTT Viewer**
* **J-Link Software**

## 🚀 Build & Run

1. Clone the project.
2. Open it in **onsemi IDE**.
3. Compile and flash the firmware to the **RSL10 EVB**.
4. Open **SEGGER RTT Viewer** to monitor logs.
5. Press **SW2** to toggle LED blinking and update event count.

## ⚙️ Implementation Notes

* **SysTick** runs every 1 ms → increments `rtc_seconds` every 1000 ticks.
* **DIO0_IRQHandler** handles button interrupts with a flag to prevent bounce.
* **RTC_PrintTimestamp()** prints time and LED state every 5 seconds.
* The system remains stable after multiple resets and power cycles.