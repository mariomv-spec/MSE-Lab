#ifndef PWM_H
#define PWM_H

#include <stdint.h>

/* =========================================================
 * pwm.h
 * PWM Module - Generación de señal PWM por hardware
 * Usa internamente el TIM driver y el GPIO driver
 *
 * Timer asignado: TIM3, Canal 1
 * Pin de salida:  PA6 (AF2 = TIM3_CH1, disponible en NUCLEO-F411RE)
 * Clock del sistema: 16 MHz (HSI por defecto)
 * ========================================================= */

/* --- Códigos de error --- */
#define PWM_OK              0
#define PWM_ERR_INVALID    -1

/**
 * @brief  Inicializa el módulo PWM.
 *         Configura TIM3 CH1 y PA6 en modo función alterna AF2.
 *         Frecuencia base configurable con pwm_setSignal().
 * @retval PWM_OK o PWM_ERR_INVALID
 */
int pwm_init(void);

/**
 * @brief  Configura la frecuencia y el duty cycle de la señal PWM.
 * @param  freq_hz:    frecuencia deseada en Hz (ej: 1000 para 1 kHz)
 * @param  duty_pct:   duty cycle en porcentaje (0-100)
 * @retval PWM_OK o PWM_ERR_INVALID
 */
int pwm_setSignal(uint32_t freq_hz, uint8_t duty_pct);

/**
 * @brief  Inicia la generación de la señal PWM (habilita el contador).
 * @retval PWM_OK o PWM_ERR_INVALID
 */
int pwm_start(void);

/**
 * @brief  Detiene la generación de la señal PWM (deshabilita el contador).
 * @retval PWM_OK o PWM_ERR_INVALID
 */
int pwm_stop(void);

#endif /* PWM_H */
