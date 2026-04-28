#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#include "GPIO_stm32.h"
#include <stdint.h>
#include <stddef.h>

/**
 * @brief  Initializes the user button (B1) on the STM32 board
 */
void button_init(void);

/**
 * @brief  Reads the state of the user button (B1).
 * @param  state Pointer to a variable where the button state will be stored.
 */
void button_get_state(uint8_t *state);

#endif /* BUTTON_DRIVER_H */


