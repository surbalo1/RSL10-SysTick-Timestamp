<div align="center">

# ⏱️ RSL10 SysTick Timestamp System

[![C](https://img.shields.io/badge/C-A8B9CC?style=for-the-badge&logo=c&logoColor=black)](https://en.wikipedia.org/wiki/C_(programming_language))
[![ARM](https://img.shields.io/badge/ARM_Cortex--M3-0091BD?style=for-the-badge&logo=arm&logoColor=white)](https://arm.com)
[![RSL10](https://img.shields.io/badge/ON_Semi_RSL10-00A651?style=for-the-badge&logo=onsemi&logoColor=white)](https://www.onsemi.com/products/wireless-connectivity/bluetooth-low-energy/rsl10)
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)

**SysTick-based timestamp system with button event counting and low-power sleep mode.**

*Millisecond precision • Hybrid debounce • SEGGER RTT debugging*

</div>

---

## 📋 Overview

A real-time timestamp and event counter system using the ARM Cortex-M3 SysTick timer. Generates timestamps, counts button presses within a 10-second window, and operates in low-power sleep mode between interrupts.

---

## ✨ Features

| Feature | Description |
|---------|-------------|
| **⏱️ Timestamps** | 1-second precision (HH:MM:SS format) |
| **🔘 Button Counter** | Tracks presses in 10-second windows |
| **💡 LED Toggle** | Visual feedback on button press |
| **🔇 Hybrid Debounce** | Hardware + software flag protection |
| **😴 Low-Power Sleep** | Idle between interrupts |
| **🔌 RTT Debug** | SEGGER RTT output (no UART needed) |

---

## 🛠️ Hardware

| Component | Pin | Function |
|-----------|:---:|----------|
| **Button SW2** | DIO5 | Input trigger |
| **LED** | DIO6 | Status output |
| **Debugger** | - | J-Link |

---

## 📊 Example Output (RTT)

```
[TIMESTAMP] 00:55:45 - LED OFF  
[CONTADOR] Button: 15 times in last 10 sec  

[GPIO] Button pressed! Total: 16 (timestamp: 3348 sec)  
[GPIO] Button pressed! Total: 17 (timestamp: 3348 sec)  
[TIMESTAMP] 00:55:50 - LED ON  
[CONTADOR] Button: 18 times in last 10 sec  
```

---

## 🚀 Quick Start

```bash
git clone https://github.com/surbalo1/RSL10-SysTick-Timestamp.git
cd RSL10-SysTick-Timestamp
# Import in onsemi IDE → Build → Flash
# Open SEGGER RTT Viewer to monitor
```

---

## ⚙️ Implementation

| Component | Details |
|-----------|---------|
| **SysTick** | 1ms tick → `rtc_seconds` every 1000 ticks |
| **DIO0_IRQHandler** | Button interrupt with debounce flag |
| **RTC_PrintTimestamp()** | Prints time every 5 seconds |

---

## 📄 License

MIT License

---

<div align="center">

[![GitHub](https://img.shields.io/badge/Star_on_GitHub-181717?style=for-the-badge&logo=github&logoColor=white)](https://github.com/surbalo1/RSL10-SysTick-Timestamp)

</div>