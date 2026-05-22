/**
 * @file    main.c
 * @brief   Main application entry point for UART communication demo.
 * @author  Adrián Rafael Amaro Avala
 *          Leonardo Ivan García Espinosa 
 *          Mario Morales Vega
 *
 * @details The program initializes UART, ADC, and GPIO peripherals, 
 *          then enters an infinite loop to read ADC values and 
 *          transmit them through the UART interface. The main function 
 *          utilizes the serial_printf function to send formatted output 
 *          to a terminal.
 * 
 *  */



#include <stdint.h>
#include <stddef.h>
#include "adc_driver.h"
#include "GPIO_stm32.h"
#include "utils.h"
#include "sensor.h"
#include "serial.h"
#include "tim_driver.h"

#define PSC     999U // Prescaler para 1 kHz (f_cnt = f_clk / (PSC + 1))
#define seconds 0.5

/* ARR = (Tiempo * f_clk / (PSC + 1)) - 1 */
#define ARR ((uint32_t)((16000000UL * seconds) / (PSC + 1)) - 1)

int main(void){

    /* Inicializar el puerto GPIOA */
    gpio_initPort(GPIO_PORT_A);

    /* Configurar el pin PA0 como entrada analógica */
    gpio_setPinMode(GPIO_PORT_A, GPIO_PIN_0, GPIO_MODE_ANALOG);

    /* Configurar el pin PA2 para comunicación serial USART2*/
    gpio_setPinMode(GPIO_PORT_A, GPIO_PIN_2, GPIO_MODE_ALT_FUNC);
    gpio_setAlternateFunction(GPIO_PORT_A, GPIO_PIN_2, GPIO_AF7); // AF7 permite USART2_TX


    /* Inicializar el módulo Serial */
    serial_init();

    /* Inicializar sensor */
    sensor_init(ADC_INSTANCE_1, ADC_CHANNEL_0);

    /* Configurar temporizador de 500 ms */
    tim_clock_enable(TIM_2);

    TIM_RegDef_t* timer = tim_get(TIM_2);

    tim_set_prescaler(timer, PSC);
    tim_set_arr(timer, ARR); 
    tim_set_arpe(timer, 1); 
    tim_start(timer);


    while(1)
    {
        /* Leer el valor del ADC conectado al sensor de luz */
        sensor_startConversion(ADC_INSTANCE_1);
        uint32_t adc_value = 0;
        sensor_readValue(ADC_INSTANCE_1, &adc_value);
        
        /* Enviar el valor leído por UART en formato legible */
        serial_printf("Valor ADC: %u\r\n", adc_value);

        /* delay de 500 ms */
        tim_wait_update(timer);
        tim_clear_update_flag(timer);
    }
    return 0;

}