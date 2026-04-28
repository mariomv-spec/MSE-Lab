/* =========================================================
 * pwm.c
 * PWM Module - Señal PWM por hardware usando TIM3 CH1
 *
 * STM32F411RE - HSI = 16 MHz
 *
 * Pin de salida: PA6 (AF2 = TIM3_CH1)
 *   - PA6 disponible en el header CN10 del NUCLEO-F411RE
 *
 * Fórmula de configuración:
 *   ARR  = (f_clk / (PSC+1) / freq_hz) - 1
 *   CCR1 = (ARR + 1) * duty_pct / 100
 *
 * Se usa PSC = 0 para máxima resolución a frecuencias altas,
 * o PSC ajustado dinámicamente para frecuencias bajas.
 * ========================================================= */

#include "pwm.h"
#include "tim_driver.h"
#include "GPIO_stm32.h"

#define PWM_TIMER_NUM   TIM_2
#define PWM_CHANNEL     1U
#define PWM_GPIO_PORT   GPIO_PORT_A
#define PWM_PIN         5U
#define PWM_AF          GPIO_AF1      /* AF1 = TIM5_CH1 en PA5 */
#define F_CLK_HZ        16000000UL

static TIM_RegDef_t *pwm_tim = 0;

/* -------------------------------------------------------
 * pwm_init
 * ------------------------------------------------------- */
int pwm_init(void)
{
    /* 1. Habilitar clock de TIM3 */
    if (tim_clock_enable(PWM_TIMER_NUM) != TIM_OK) {
        return PWM_ERR_INVALID;
    }

    /* 2. Obtener puntero al timer */
    pwm_tim = tim_get(PWM_TIMER_NUM);
    if (pwm_tim == 0) {
        return PWM_ERR_INVALID;
    }

    /* 3. Asegurar que el timer esté detenido */
    tim_stop(pwm_tim);

    /* 4. Configurar PA5 en modo función alterna */
    gpio_initPort(GPIO_PORT_A);
    gpio_setPinMode(GPIO_PORT_A, GPIO_PIN_5, GPIO_MODE_ALT_FUNC);
    gpio_setAlternateFunction(GPIO_PORT_A, GPIO_PIN_5, PWM_AF);

    /* 5. Configurar TIM2 CH1 en modo PWM 1 */
    tim_set_pwm_mode(pwm_tim, PWM_CHANNEL, TIM_PWM_MODE1);

    /* 6. Habilitar salida del canal */
    tim_channel_output_enable(pwm_tim, PWM_CHANNEL, 1);

    /* 7. Habilitar ARPE */
    tim_set_arpe(pwm_tim, 1);

    /* 8. Valores iniciales */
    tim_set_prescaler(pwm_tim, 0);
    tim_set_arr(pwm_tim, 999);
    tim_set_ccr(pwm_tim, PWM_CHANNEL, 499);

    pwm_tim->EGR |= (1U << 0);   // Genera Update Event manual (bit UG)
    pwm_tim->SR &= ~(1U << 0);   // Limpia la bandera UIF levantada

    return PWM_OK;
}

/* -------------------------------------------------------
 * pwm_setSignal
 * ------------------------------------------------------- */
int pwm_setSignal(uint32_t freq_hz, uint8_t duty_pct)
{
    if (pwm_tim == 0 || freq_hz == 0 || duty_pct > 100) {
        return PWM_ERR_INVALID;
    }

    /* Calcular PSC y ARR para la frecuencia deseada.
     * Se busca el PSC mínimo tal que ARR <= 65535 (TIM3 es 16-bit). */
    uint32_t psc  = 0;
    uint32_t arr  = 0;

    do {
        arr = (F_CLK_HZ / (psc + 1U) / freq_hz) - 1U;
        if (arr <= 65535UL) break;
        psc++;
    } while (psc < 65535U);

    if (arr > 65535UL) {
        return PWM_ERR_INVALID;  /* Frecuencia demasiado baja para 16-bit */
    }

    uint32_t ccr = ((arr + 1U) * duty_pct) / 100U;

    tim_set_prescaler(pwm_tim, (uint16_t)psc);
    tim_set_arr(pwm_tim, arr);
    tim_set_ccr(pwm_tim, PWM_CHANNEL, ccr);

    return PWM_OK;
}

/* -------------------------------------------------------
 * pwm_start
 * ------------------------------------------------------- */
int pwm_start(void)
{
    if (pwm_tim == 0) {
        return PWM_ERR_INVALID;
    }
    return (tim_start(pwm_tim) == TIM_OK) ? PWM_OK : PWM_ERR_INVALID;
}

/* -------------------------------------------------------
 * pwm_stop
 * ------------------------------------------------------- */
int pwm_stop(void)
{
    if (pwm_tim == 0) {
        return PWM_ERR_INVALID;
    }
    return (tim_stop(pwm_tim) == TIM_OK) ? PWM_OK : PWM_ERR_INVALID;
}
