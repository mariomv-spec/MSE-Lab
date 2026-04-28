/* =========================================================
 * timer.c
 * Timer Module - Delay por hardware usando TIM2
 *
 * STM32F411RE - HSI = 16 MHz
 *
 * Configuración:
 *   PSC = 15999  => f_cnt = 16MHz / (15999+1) = 1000 Hz => 1 tick = 1 ms
 *   ARR = 0      => se actualiza dinámicamente en timer_delay_ms
 *
 * Estrategia de delay:
 *   Para N ms, se configura ARR = N-1, se inicia el contador,
 *   se espera el flag UIF (overflow) y se detiene el timer.
 * ========================================================= */

#include "timer.h"
#include "tim_driver.h"

#define DELAY_TIMER_NUM     TIM_5
#define DELAY_PSC           15999U   /* 16 MHz / 16000 = 1 kHz */

static TIM_RegDef_t *delay_tim = 0;

/* -------------------------------------------------------
 * timer_init
 * ------------------------------------------------------- */
int timer_init(void)
{
    /* 1. Habilitar clock de TIM2 */
    if (tim_clock_enable(DELAY_TIMER_NUM) != TIM_OK) {
        return TIMER_ERR_INVALID;
    }

    /* 2. Obtener puntero al timer */
    delay_tim = tim_get(DELAY_TIMER_NUM);
    if (delay_tim == 0) {
        return TIMER_ERR_INVALID;
    }

    /* 3. Asegurarse de que el contador esté detenido */
    tim_stop(delay_tim);

    /* 4. Configurar prescaler: 1 tick = 1 ms */
    tim_set_prescaler(delay_tim, (uint16_t)DELAY_PSC);

    /* 5. Habilitar preload del ARR */
    tim_set_arpe(delay_tim, 0);

    /* 6. Limpiar bandera UIF por si acaso */
    tim_clear_update_flag(delay_tim);

    delay_tim->EGR |= (1U << 0);    // Genera un Update Event manual (bit UG)
    delay_tim->SR &= ~(1U << 0);    // Limpiamos la bandera UIF que el EGR acaba de levantar

    return TIMER_OK;
}

/* -------------------------------------------------------
 * timer_delay_ms
 * ------------------------------------------------------- */
int timer_delay_ms(uint32_t ms)
{
    if (delay_tim == 0 || ms == 0) {
        return TIMER_ERR_INVALID;
    }

    /* Configurar ARR = ms - 1 para que el timer genere
     * un overflow después de exactamente 'ms' ticks */
    tim_set_arr(delay_tim, ms - 1U);

    /* Reiniciar contador */
    delay_tim->CNT = 0;

    /* Limpiar flag anterior */
    tim_clear_update_flag(delay_tim);

    /* Iniciar contador */
    tim_start(delay_tim);

    /* Esperar overflow (UIF) */
    tim_wait_update(delay_tim);

    /* Detener y limpiar */
    tim_stop(delay_tim);
    tim_clear_update_flag(delay_tim);

    return TIMER_OK;
}
