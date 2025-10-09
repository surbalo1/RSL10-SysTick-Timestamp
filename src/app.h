/* ----------------------------------------------------------------------------
 * Copyright (c) 2017 Semiconductor Components Industries, LLC
 * (d/b/a ON Semiconductor). All Rights Reserved.
 *
 * Modified in 2025 by Rafael Ignacio González Chong
 * for educational and open-source purposes.
 * ----------------------------------------------------------------------------
 * app.h
 * - Main application header for the SysTick timestamp example on RSL10
 * --------------------------------------------------------------------------*/

#ifndef APP_H
#define APP_H

#include <rsl10.h>

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/

/* GPIO mapping for RSL10 EVB */
#define BUTTON_DIO                      5   /* SW2 input */
#define LED_DIO                         6   /* LED output */
#define RECOVERY_DIO                    12  /* Recovery mode pin */

/* RTC configuration */
#define RTC_ALARM_INTERVAL_SEC          5
#define BUTTON_EVENT_WINDOW_SEC         10

/* Macro utilities */
#define CONCAT(x, y)                    x##y
#define DIO_SRC(x)                      CONCAT(DIO_SRC_DIO_, x)

/* ----------------------------------------------------------------------------
 * Function Prototypes
 * --------------------------------------------------------------------------*/

/* Interrupt handlers */
extern void SysTick_Handler(void);
extern void DIO0_IRQHandler(void);

/* Application functions */
void Initialize(void);
void SysTickConfigLaunch(void);
void RTC_PrintTimestamp(void);
uint16_t RTC_GetButtonCountLastNSeconds(uint8_t n_seconds);

#endif /* APP_H */
