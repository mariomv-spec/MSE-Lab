#include "led.h"
#include "timer.h"
#include "pwm.h"
#include "button_driver.h"
#include "GPIO_stm32.h"

#define S0 0U
#define S1 1U

typedef enum {
        ESPERAR_PRESION,
        ACTUALIZAR_BRILLO,
        ESPERAR_SOLTAR
} EstadoBoton_t;

int main(void)
{
    // 1. Inicializar Hardware
    gpio_init();
    timer_init(); 
    pwm_init();   
    button_init();

    pwm_start();

    EstadoBoton_t state = ESPERAR_PRESION;
    uint8_t button_state = 0;
    uint8_t brillo_pct = 0;

    while (1)
    {
        // Se modifica el duty cycle
        pwm_setSignal(1000U, brillo_pct);

        // Se lee el estado del botón
        button_get_state(&button_state);

        // Máquina de estados
        switch (state) {
            case ESPERAR_PRESION:
                if (button_state == 1) { // Si se presionó se actualiza el brillo y se pasa al siguiente estado
                    state = ACTUALIZAR_BRILLO;
                }
                break;

            case ACTUALIZAR_BRILLO:

                // Se incrementa en un 10% el duty cycle, volviendo a 0% si se supera el 100%
                brillo_pct += 10U;
                if (brillo_pct > 100U) {
                    brillo_pct = 0U;
                }
                
                // Se espera a que se suelte el botón antes de hacer otro cambio
                state = ESPERAR_SOLTAR; 
                break;

            case ESPERAR_SOLTAR:

                // Si el botón se suelta, se vuelve al estado de espera de presión
                if (button_state == 0) { 
                    state = ESPERAR_PRESION; 
                }
                break;

            default:
                state = ESPERAR_PRESION;
                break;
        }
    }
    return 0;
}