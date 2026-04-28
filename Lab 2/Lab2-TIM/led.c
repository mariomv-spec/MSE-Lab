#include "led.h"
#include "GPIO_stm32.h"

#define LED_PORT    GPIO_PORT_A
#define LED_PIN     GPIO_PIN_5

void led_init(void)
{
    gpio_initPort(LED_PORT);
    gpio_setPinMode(LED_PORT, LED_PIN, GPIO_MODE_OUTPUT);
    gpio_clearPin(LED_PORT, LED_PIN);
}

void led_on(void)
{
    gpio_setPin(LED_PORT, LED_PIN);
}

void led_off(void)
{
    gpio_clearPin(LED_PORT, LED_PIN);
}

void led_toggle(void)
{
    gpio_togglePin(LED_PORT, LED_PIN);
}