/* =========================================================
 * tim_driver.c
 * TIM Driver - STM32F411RE
 * Bare-metal, sin HAL
 * ========================================================= */

#include "tim_driver.h"

/* -------------------------------------------------------
 * tim_clock_enable  (FR-1)
 * ------------------------------------------------------- */
int tim_clock_enable(uint8_t tim_num)
{
    switch (tim_num) {
        case TIM_1: RCC_APB2ENR |= RCC_APB2ENR_TIM1EN; break;
        case TIM_2: RCC_APB1ENR |= RCC_APB1ENR_TIM2EN; break;
        case TIM_3: RCC_APB1ENR |= RCC_APB1ENR_TIM3EN; break;
        case TIM_4: RCC_APB1ENR |= RCC_APB1ENR_TIM4EN; break;
        case TIM_5: RCC_APB1ENR |= RCC_APB1ENR_TIM5EN; break;
        default: return TIM_ERR_INVALID;
    }
    return TIM_OK;
}

/* -------------------------------------------------------
 * tim_get  (FR-2)
 * ------------------------------------------------------- */
TIM_RegDef_t *tim_get(uint8_t tim_num)
{
    switch (tim_num) {
        case TIM_1: return TIM1;
        case TIM_2: return TIM2;
        case TIM_3: return TIM3;
        case TIM_4: return TIM4;
        case TIM_5: return TIM5;
        default:    return 0;
    }
}

/* -------------------------------------------------------
 * tim_set_prescaler  (FR-3)
 * ------------------------------------------------------- */
int tim_set_prescaler(TIM_RegDef_t *TIMx, uint16_t psc)
{
    if (TIMx == 0) {
        return TIM_ERR_INVALID;
    }
    TIMx->PSC = psc;
    return TIM_OK;
}

/* -------------------------------------------------------
 * tim_set_arr  (FR-4)
 * ------------------------------------------------------- */
int tim_set_arr(TIM_RegDef_t *TIMx, uint32_t arr)
{
    if (TIMx == 0) {
        return TIM_ERR_INVALID;
    }
    TIMx->ARR = arr;
    return TIM_OK;
}

/* -------------------------------------------------------
 * tim_set_arpe  (FR-5)
 * ------------------------------------------------------- */
int tim_set_arpe(TIM_RegDef_t *TIMx, uint8_t enable)
{
    if (TIMx == 0) {
        return TIM_ERR_INVALID;
    }

    if (enable) {
        TIMx->CR1 |= TIM_CR1_ARPE;
    } else {
        TIMx->CR1 &= ~TIM_CR1_ARPE;
    }
    return TIM_OK;
}

/* -------------------------------------------------------
 * tim_start  (FR-6)
 * ------------------------------------------------------- */
int tim_start(TIM_RegDef_t *TIMx)
{
    if (TIMx == 0) {
        return TIM_ERR_INVALID;
    }
    TIMx->CR1 |= TIM_CR1_CEN;
    return TIM_OK;
}

/* -------------------------------------------------------
 * tim_stop  (FR-7)
 * ------------------------------------------------------- */
int tim_stop(TIM_RegDef_t *TIMx)
{
    if (TIMx == 0) {
        return TIM_ERR_INVALID;
    }
    TIMx->CR1 &= ~TIM_CR1_CEN;
    return TIM_OK;
}

/* -------------------------------------------------------
 * tim_wait_update  (FR-8)
 * Polling bloqueante hasta que el flag UIF se active
 * ------------------------------------------------------- */
int tim_wait_update(TIM_RegDef_t *TIMx)
{
    if (TIMx == 0) {
        return TIM_ERR_INVALID;
    }
    while (!(TIMx->SR & TIM_SR_UIF)) {
        /* esperar */
    }
    return TIM_OK;
}

/* -------------------------------------------------------
 * tim_clear_update_flag  (FR-9)
 * ------------------------------------------------------- */
int tim_clear_update_flag(TIM_RegDef_t *TIMx)
{
    if (TIMx == 0) {
        return TIM_ERR_INVALID;
    }
    TIMx->SR &= ~TIM_SR_UIF;
    return TIM_OK;
}

/* -------------------------------------------------------
 * tim_set_pwm_mode  (FR-10)
 *
 * CCMR1 controla canales 1 y 2
 * CCMR2 controla canales 3 y 4
 *
 * Para cada canal en CCMRx:
 *   bits [2:0]  = CCxS  (00 = output)
 *   bits [6:4]  = OCxM  (110 = PWM1, 111 = PWM2)
 *   bit  [3]    = OCxPE (preload enable)
 * ------------------------------------------------------- */
int tim_set_pwm_mode(TIM_RegDef_t *TIMx, uint8_t channel, uint8_t mode)
{
    if (TIMx == 0 || channel < 1 || channel > 4 || (mode != TIM_PWM_MODE1 && mode != TIM_PWM_MODE2)) {
        return TIM_ERR_INVALID;
    }

    switch (channel) {
        case 1:
            /* CCxS = 00 (output), OCxM = mode, OCxPE = 1 */
            TIMx->CCMR1 &= ~(0xFFU << 0);
            TIMx->CCMR1 |= ((uint32_t)mode << 4) | (1U << 3);
            break;
        case 2:
            TIMx->CCMR1 &= ~(0xFFU << 8);
            TIMx->CCMR1 |= ((uint32_t)mode << 12) | (1U << 11);
            break;
        case 3:
            TIMx->CCMR2 &= ~(0xFFU << 0);
            TIMx->CCMR2 |= ((uint32_t)mode << 4) | (1U << 3);
            break;
        case 4:
            TIMx->CCMR2 &= ~(0xFFU << 8);
            TIMx->CCMR2 |= ((uint32_t)mode << 12) | (1U << 11);
            break;
        default:
            return TIM_ERR_INVALID;
    }
    return TIM_OK;
}

/* -------------------------------------------------------
 * tim_set_ccr  (FR-11)
 * ------------------------------------------------------- */
int tim_set_ccr(TIM_RegDef_t *TIMx, uint8_t channel, uint32_t ccr)
{
    if (TIMx == 0 || channel < 1 || channel > 4) {
        return TIM_ERR_INVALID;
    }

    switch (channel) {
        case 1: TIMx->CCR1 = ccr; break;
        case 2: TIMx->CCR2 = ccr; break;
        case 3: TIMx->CCR3 = ccr; break;
        case 4: TIMx->CCR4 = ccr; break;
        default: return TIM_ERR_INVALID;
    }
    return TIM_OK;
}

/* -------------------------------------------------------
 * tim_channel_output_enable
 * ------------------------------------------------------- */
int tim_channel_output_enable(TIM_RegDef_t *TIMx, uint8_t channel, uint8_t enable)
{
    if (TIMx == 0 || channel < 1 || channel > 4) {
        return TIM_ERR_INVALID;
    }

    uint32_t ccer_bit = 0;
    switch (channel) {
        case 1: ccer_bit = TIM_CCER_CC1E; break;
        case 2: ccer_bit = TIM_CCER_CC2E; break;
        case 3: ccer_bit = TIM_CCER_CC3E; break;
        case 4: ccer_bit = TIM_CCER_CC4E; break;
    }

    if (enable) {
        TIMx->CCER |= ccer_bit;
    } else {
        TIMx->CCER &= ~ccer_bit;
    }
    return TIM_OK;
}

/* -------------------------------------------------------
 * tim_bdtr_moe_enable
 * Solo para TIM1 (timer avanzado): habilita MOE en BDTR
 * ------------------------------------------------------- */
int tim_bdtr_moe_enable(TIM_RegDef_t *TIMx)
{
    if (TIMx == 0) {
        return TIM_ERR_INVALID;
    }
    /* MOE = bit 15 de BDTR */
    TIMx->BDTR |= (1U << 15);
    return TIM_OK;
}
