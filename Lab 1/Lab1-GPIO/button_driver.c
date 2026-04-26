#include "button_driver.h"
#include "GPIO_stm32.h"
#include <stdint.h>
#include <stddef.h>

#define BUTTON_PORT     GPIO_PORT_C
#define BUTTON_PIN      GPIO_PIN_13
#define READ_MASK       1U


void button_init(void){

    gpio_initPort(BUTTON_PORT);
    gpio_setPinMode(BUTTON_PORT, BUTTON_PIN, GPIO_MODE_INPUT);

}

void button_get_state(uint8_t *state){

    if (state == NULL) {
        return; 
    }

    gpio_readPin(BUTTON_PORT, BUTTON_PIN, state);

}