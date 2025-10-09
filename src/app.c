#include "app.h"
#include <printf.h>
#include "SEGGER_RTT.h"

/* Global variables used for time, button tracking, and LED control */
volatile uint32_t rtc_seconds = 0;
volatile uint8_t rtc_alarm_flag = 0;
volatile uint16_t button_press_count = 0;
volatile uint32_t last_button_time = 0;
volatile uint8_t led_toggle_status = 0;
volatile uint16_t systick_ms_counter = 0;

/* This handler runs every 1ms. It keeps track of time using a millisecond counter.
 * Every 1000ms (1 second), it updates the RTC seconds and sets a flag.
 * Also toggles the LED every second if blinking is enabled.
 */
void SysTick_Handler(void)
{
    systick_ms_counter++;

    /* 1000ms = 1 second */
    if (systick_ms_counter >= 1000) {
        systick_ms_counter = 0;
        rtc_seconds++;
        rtc_alarm_flag = 1;
    }

    /* Toggle LED every second if enabled */
    if (led_toggle_status == 1 && (systick_ms_counter % 1000) == 0) {
        Sys_GPIO_Toggle(LED_DIO);
        PRINTF("LED_%s\n", (DIO->CFG[LED_DIO] & 0x1 ? "ON" : "OFF"));
    }
}

/* This interrupt runs when the user presses the button.
 * It toggles the LED blinking state and counts how many times the button was pressed.
 * I added a small ignore flag to prevent double triggers due to bounce.
 */
void DIO0_IRQHandler(void)
{
    static uint8_t ignore_next_dio_int = 0;

    if (ignore_next_dio_int)
    {
        ignore_next_dio_int = 0;
    }
    else if (DIO_DATA->ALIAS[BUTTON_DIO] == 0)
    {
        ignore_next_dio_int = 1;

        /* Toggle LED blink mode */
        if (led_toggle_status == 1) {
            led_toggle_status = 0;
            Sys_GPIO_Set_Low(LED_DIO);
            PRINTF("LED_BLINK_DISABLED\n");
        } else {
            led_toggle_status = 1;
            PRINTF("LED_BLINK_ENABLED\n");
        }

        /* Increase button counter */
        button_press_count++;
        last_button_time = rtc_seconds;

        SEGGER_RTT_printf(0, "[GPIO] Button pressed! Total: %d (timestamp: %lu sec)\n",
                          button_press_count, last_button_time);
    }
}

/* Prints the current time in HH:MM:SS format using the total seconds counter.
 * Helpful for checking if the timer is working correctly.
 */
void RTC_PrintTimestamp(void) {
    uint16_t hours = (rtc_seconds / 3600) % 24;
    uint8_t minutes = (rtc_seconds / 60) % 60;
    uint8_t seconds = rtc_seconds % 60;

    SEGGER_RTT_printf(0, "[TIMESTAMP] %02d:%02d:%02d - LED %s\n",
                      hours, minutes, seconds,
                      (DIO->CFG[LED_DIO] & 0x1 ? "ON" : "OFF"));
}

/* Returns how many times the button was pressed within the last N seconds.
 * If the time window passed, it resets the counter.
 */
uint16_t RTC_GetButtonCountLastNSeconds(uint8_t n_seconds) {
    if ((rtc_seconds > last_button_time) &&
        ((rtc_seconds - last_button_time) > n_seconds)) {
        button_press_count = 0;
        return 0;
    } else {
        return button_press_count;
    }
}

/* Configures the SysTick timer to trigger every 1ms.
 * This is used as the base for timing and LED blinking.
 */
void SysTickConfigLaunch(void)
{
    /* 1. Disable timer */
    SysTick->CTRL = 0;

    /* 2. Set reload value for 1ms tick
     * SystemCoreClock / 1000 = ticks per millisecond
     */
    SysTick->LOAD = SystemCoreClock / 1000;
    PRINTF("SYSTICK_LOAD_VALUE=%d (1ms tick)\n", SysTick->LOAD);

    /* 3. Clear counter */
    SysTick->VAL  = 0;

    /* 4. Start timer with system clock */
    SysTick->CTRL = ((1 << SysTick_CTRL_CLKSOURCE_Pos) |
                     (1 << SysTick_CTRL_TICKINT_Pos)   |
                     (1 << SysTick_CTRL_ENABLE_Pos));
}

/* Initializes all GPIOs, NVIC, and RTT.
 * Also waits for a recovery pin (DIO12) to be released before running,
 * so I can reflash easily if something goes wrong.
 */
void Initialize(void)
{
    /* Mask all interrupts */
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);

    /* Disable and clear interrupts */
    Sys_NVIC_DisableAllInt();
    Sys_NVIC_ClearAllPendingInt();

    /* Recovery pin setup */
    DIO->CFG[RECOVERY_DIO] = DIO_MODE_INPUT  | DIO_WEAK_PULL_UP |
                             DIO_LPF_DISABLE | DIO_6X_DRIVE;
    while (DIO_DATA->ALIAS[RECOVERY_DIO] == 0);

    /* LED and Button setup */
    Sys_DIO_Config(LED_DIO, DIO_MODE_GPIO_OUT_0);
    Sys_DIO_Config(BUTTON_DIO, DIO_MODE_GPIO_IN_0 | DIO_WEAK_PULL_UP |
                   DIO_LPF_DISABLE);

    Sys_DIO_IntConfig(0,
                      DIO_EVENT_TRANSITION | DIO_SRC(BUTTON_DIO) |
                      DIO_DEBOUNCE_ENABLE,
                      DIO_DEBOUNCE_SLOWCLK_DIV1024, 99);

    /* DIO interrupt priority */
    NVIC_SetPriority(DIO0_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
    NVIC_EnableIRQ(DIO0_IRQn);

    /* Initialize RTT for debug messages */
    SEGGER_RTT_Init();
    SEGGER_RTT_printf(0, "\n========== RSL10 SysTick Timer + Timestamp Test ==========\n");

    /* Enable interrupts again */
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
}

/* Main loop keeps refreshing the watchdog, prints time every 5 seconds,
 * and goes to sleep while waiting for interrupts.
 * The LED blinking and button logic run through SysTick and DIO interrupts.
 */
int main(void)
{
    /* System init */
    Initialize();
    PRINTF("DEVICE_INITIALIZED\n");

    /* Setup SysTick timer */
    SysTickConfigLaunch();

    led_toggle_status = 1;
    uint32_t last_print_time = 0;

    /* Infinite loop */
    while (1)
    {
        /* Refresh watchdog timer */
        Sys_Watchdog_Refresh();

        /* Print timestamp every 5 seconds */
        if (rtc_alarm_flag && ((rtc_seconds - last_print_time) >= RTC_ALARM_INTERVAL_SEC)) {
            rtc_alarm_flag = 0;
            last_print_time = rtc_seconds;

            RTC_PrintTimestamp();
            uint16_t count = RTC_GetButtonCountLastNSeconds(BUTTON_EVENT_WINDOW_SEC);
            SEGGER_RTT_printf(0, "[COUNT] Button: %d times in last %d sec\n\n",
                              count, BUTTON_EVENT_WINDOW_SEC);
        }

        /* Sleep until next interrupt */
        __WFI();
    }
}
