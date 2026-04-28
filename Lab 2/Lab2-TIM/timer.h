#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

/* =========================================================
 * timer.h
 * Timer Module - Generación de delays por hardware
 * Usa internamente el TIM driver
 *
 * Timer asignado: TIM2 (APB1, 32-bit)
 * Clock del sistema: 16 MHz (HSI por defecto en NUCLEO-F411RE)
 * ========================================================= */

/* --- Códigos de error --- */
#define TIMER_OK            0
#define TIMER_ERR_INVALID  -1

/**
 * @brief  Inicializa el timer de delay (TIM2).
 *         Configura PSC y ARR para base de 1 ms.
 *         f_clk = 16 MHz, PSC = 15999, ARR = 0xFFFFFFFF
 *         => f_cnt = 1 kHz => cada tick = 1 ms
 * @retval TIMER_OK o TIMER_ERR_INVALID
 */
int timer_init(void);

/**
 * @brief  Genera un delay bloqueante de N milisegundos.
 * @param  ms: cantidad de milisegundos a esperar
 * @retval TIMER_OK o TIMER_ERR_INVALID
 */
int timer_delay_ms(uint32_t ms);

#endif /* TIMER_H */
