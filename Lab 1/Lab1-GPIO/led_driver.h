#ifndef LED_H
#define LED_H

/* =========================================================
 * led.h
 * LED Module - Control del LED de usuario LD2
 *
 * En NUCLEO-F411RE, LD2 está conectado a PA5.
 * ========================================================= */

/**
 * @brief  Inicializa PA5 como salida push-pull para el LED LD2.
 */
void led_init(void);

/**
 * @brief  Enciende el LED LD2.
 */
void led_on(void);

/**
 * @brief  Apaga el LED LD2.
 */
void led_off(void);

/**
 * @brief  Invierte el estado del LED LD2.
 */
void led_toggle(void);

#endif /* LED_H */
