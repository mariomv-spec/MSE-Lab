/* @file    main.c
 * @brief   ADC Driver Application – Sensor-Controlled PWM
 *
 * @details Reads a potentiometer connected to PA0 (ADC1 CH0) and maps
 *          the 12-bit ADC result (0–4095) to a PWM duty cycle (0–100%)
 *          on PA5 (TIM2 CH1 – onboard LED LD2).
 *
 * @board   STM32F411RE Nucleo-64
 * @author  Adrián Rafael Amaro Avala
 *          Leonardo Ivan García Espinosa 
 *          Mario Morales Vega
 * @date    April 29, 2026
 *
 * Pin Mapping
 * -----------
 *  Potentiometer wiper  →  PA0  (ADC1_IN0,  CN7 pin 28 / Arduino A0)
 *  PWM output / LD2     →  PA5  (TIM2_CH1,  CN10 pin 11 / Arduino D13)
 *
 * To use a different timer/channel, replace the TIM2 / PWM_CHANNEL_1
 * literals below and update the GPIO alternate-function call accordingly.
 */
 
/* -----------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------- */
#include <stdint.h>
#include "GPIO_stm32.h"
#include "sensor.h"
#include "pwm.h"

int main(void) {
    uint32_t adc_raw_value = 0;
    volatile uint32_t delay_count;

    /* 1. Inicialización de Periféricos (Asumiendo inicialización exitosa) */
    gpio_initPort(GPIO_PORT_A);
    gpio_setPinMode(GPIO_PORT_A, GPIO_PIN_0, GPIO_MODE_ANALOG); // PA0 analógico
    
    sensor_init(ADC_INSTANCE_1, ADC_CHANNEL_0);
    pwm_init();
    pwm_start();

    /* 2. Bucle Principal */
    while (1) {
        /* Iniciar y leer conversión ADC */
        if (sensor_startConversion(ADC_INSTANCE_1) == SENSOR_OK) {
            if (sensor_readValue(ADC_INSTANCE_1, &adc_raw_value) == SENSOR_OK) {
                
                /* Mapeo directo de 12 bits (0-4095) a Duty Cycle (0-100%)
                 * y actualización del PWM a 1kHz (1000 Hz) en una sola línea */
                pwm_setSignal(1000U, (uint8_t)((adc_raw_value * 100U) / 4095U));
            }
        }

        /* Retardo simple en línea para estabilidad (~300us) */
        for (delay_count = 0U; delay_count < 30000U ; delay_count++) {
            __asm__("nop");
        }
    }

    return 0;
}