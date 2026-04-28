#ifndef TIM_DRIVER_H
#define TIM_DRIVER_H

#include <stdint.h>

/* =========================================================
 * tim_driver.h
 * TIM Driver - STM32F411RE (NUCLEO-F411RE)
 * Bare-metal, sin HAL
 *
 * Timers soportados: TIM1, TIM2, TIM3, TIM4, TIM5
 * Canales soportados: 1 - 4
 * ========================================================= */

/* --- Direcciones base de los timers --- */
#define TIM1_BASE   0x40010000UL
#define TIM2_BASE   0x40000000UL
#define TIM3_BASE   0x40000400UL
#define TIM4_BASE   0x40000800UL
#define TIM5_BASE   0x40000C00UL

/* --- RCC para habilitar clocks de timers --- */
#ifndef RCC_BASE
#define RCC_BASE        0x40023800UL
#endif
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x40))
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x44))

#define RCC_APB1ENR_TIM2EN  (1U << 0)
#define RCC_APB1ENR_TIM3EN  (1U << 1)
#define RCC_APB1ENR_TIM4EN  (1U << 2)
#define RCC_APB1ENR_TIM5EN  (1U << 3)
#define RCC_APB2ENR_TIM1EN  (1U << 0)

/* --- Estructura de registros de un timer general --- */
typedef struct {
    volatile uint32_t CR1;      /* 0x00 - Control register 1         */
    volatile uint32_t CR2;      /* 0x04 - Control register 2         */
    volatile uint32_t SMCR;     /* 0x08 - Slave mode control register*/
    volatile uint32_t DIER;     /* 0x0C - DMA/interrupt enable reg   */
    volatile uint32_t SR;       /* 0x10 - Status register            */
    volatile uint32_t EGR;      /* 0x14 - Event generation register  */
    volatile uint32_t CCMR1;    /* 0x18 - Capture/compare mode 1     */
    volatile uint32_t CCMR2;    /* 0x1C - Capture/compare mode 2     */
    volatile uint32_t CCER;     /* 0x20 - Capture/compare enable reg */
    volatile uint32_t CNT;      /* 0x24 - Counter                    */
    volatile uint32_t PSC;      /* 0x28 - Prescaler                  */
    volatile uint32_t ARR;      /* 0x2C - Auto-reload register       */
    volatile uint32_t RCR;      /* 0x30 - Repetition counter (TIM1)  */
    volatile uint32_t CCR1;     /* 0x34 - Capture/compare register 1 */
    volatile uint32_t CCR2;     /* 0x38 - Capture/compare register 2 */
    volatile uint32_t CCR3;     /* 0x3C - Capture/compare register 3 */
    volatile uint32_t CCR4;     /* 0x40 - Capture/compare register 4 */
    volatile uint32_t BDTR;     /* 0x44 - Break/dead-time (TIM1)     */
    volatile uint32_t DCR;      /* 0x48 - DMA control register       */
    volatile uint32_t DMAR;     /* 0x4C - DMA address register       */
} TIM_RegDef_t;

/* --- Punteros a los timers --- */
#define TIM1    ((TIM_RegDef_t *) TIM1_BASE)
#define TIM2    ((TIM_RegDef_t *) TIM2_BASE)
#define TIM3    ((TIM_RegDef_t *) TIM3_BASE)
#define TIM4    ((TIM_RegDef_t *) TIM4_BASE)
#define TIM5    ((TIM_RegDef_t *) TIM5_BASE)

/* --- Índices de timers (para funciones del driver) --- */
#define TIM_1   1U
#define TIM_2   2U
#define TIM_3   3U
#define TIM_4   4U
#define TIM_5   5U

/* --- Bits del registro CR1 --- */
#define TIM_CR1_CEN     (1U << 0)   /* Counter enable          */
#define TIM_CR1_UDIS    (1U << 1)   /* Update disable          */
#define TIM_CR1_URS     (1U << 2)   /* Update request source   */
#define TIM_CR1_OPM     (1U << 3)   /* One-pulse mode          */
#define TIM_CR1_ARPE    (1U << 7)   /* Auto-reload preload     */

/* --- Bits del registro SR --- */
#define TIM_SR_UIF      (1U << 0)   /* Update interrupt flag   */

/* --- Bits del registro CCER --- */
#define TIM_CCER_CC1E   (1U << 0)
#define TIM_CCER_CC2E   (1U << 4)
#define TIM_CCER_CC3E   (1U << 8)
#define TIM_CCER_CC4E   (1U << 12)

/* --- Modos PWM para CCMRx --- */
#define TIM_PWM_MODE1   0x6U    /* PWM mode 1: activo mientras CNT < CCR */
#define TIM_PWM_MODE2   0x7U    /* PWM mode 2: activo mientras CNT > CCR */

/* --- Códigos de error --- */
#define TIM_OK              0
#define TIM_ERR_INVALID    -1

/* =========================================================
 * Prototipos de funciones - TIM Driver
 * ========================================================= */

/**
 * @brief  Habilita el clock del timer especificado. (FR-1)
 * @param  tim_num: TIM_1 ... TIM_5
 * @retval TIM_OK o TIM_ERR_INVALID
 */
int tim_clock_enable(uint8_t tim_num);

/**
 * @brief  Retorna el puntero al registro del timer. (FR-2)
 * @param  tim_num: TIM_1 ... TIM_5
 * @retval Puntero a TIM_RegDef_t, o NULL si inválido
 */
TIM_RegDef_t *tim_get(uint8_t tim_num);

/**
 * @brief  Configura el prescaler del timer. (FR-3)
 *         f_cnt = f_clk / (PSC + 1)
 * @param  TIMx:  puntero al timer
 * @param  psc:   valor del prescaler (0-65535)
 * @retval TIM_OK o TIM_ERR_INVALID
 */
int tim_set_prescaler(TIM_RegDef_t *TIMx, uint16_t psc);

/**
 * @brief  Configura el auto-reload register (período). (FR-4)
 *         f_update = f_cnt / (ARR + 1)
 * @param  TIMx: puntero al timer
 * @param  arr:  valor del auto-reload (0-65535 ó 0-0xFFFFFFFF en TIM2/TIM5)
 * @retval TIM_OK o TIM_ERR_INVALID
 */
int tim_set_arr(TIM_RegDef_t *TIMx, uint32_t arr);

/**
 * @brief  Habilita el auto-reload preload (ARPE bit en CR1). (FR-5)
 * @param  TIMx:   puntero al timer
 * @param  enable: 1 = habilitar, 0 = deshabilitar
 * @retval TIM_OK o TIM_ERR_INVALID
 */
int tim_set_arpe(TIM_RegDef_t *TIMx, uint8_t enable);

/**
 * @brief  Inicia el contador del timer (CEN = 1). (FR-6)
 * @param  TIMx: puntero al timer
 * @retval TIM_OK o TIM_ERR_INVALID
 */
int tim_start(TIM_RegDef_t *TIMx);

/**
 * @brief  Detiene el contador del timer (CEN = 0). (FR-7)
 * @param  TIMx: puntero al timer
 * @retval TIM_OK o TIM_ERR_INVALID
 */
int tim_stop(TIM_RegDef_t *TIMx);

/**
 * @brief  Espera hasta que el flag UIF se active (overflow). (FR-8)
 * @param  TIMx: puntero al timer
 * @retval TIM_OK o TIM_ERR_INVALID
 */
int tim_wait_update(TIM_RegDef_t *TIMx);

/**
 * @brief  Limpia el flag de actualización (UIF). (FR-9)
 * @param  TIMx: puntero al timer
 * @retval TIM_OK o TIM_ERR_INVALID
 */
int tim_clear_update_flag(TIM_RegDef_t *TIMx);

/**
 * @brief  Configura un canal en modo PWM. (FR-10)
 * @param  TIMx:    puntero al timer
 * @param  channel: 1-4
 * @param  mode:    TIM_PWM_MODE1 o TIM_PWM_MODE2
 * @retval TIM_OK o TIM_ERR_INVALID
 */
int tim_set_pwm_mode(TIM_RegDef_t *TIMx, uint8_t channel, uint8_t mode);

/**
 * @brief  Establece el valor del CCR (duty cycle). (FR-11)
 * @param  TIMx:    puntero al timer
 * @param  channel: 1-4
 * @param  ccr:     valor de comparación
 * @retval TIM_OK o TIM_ERR_INVALID
 */
int tim_set_ccr(TIM_RegDef_t *TIMx, uint8_t channel, uint32_t ccr);

/**
 * @brief  Habilita la salida del canal de captura/comparación.
 * @param  TIMx:    puntero al timer
 * @param  channel: 1-4
 * @param  enable:  1 = habilitar, 0 = deshabilitar
 * @retval TIM_OK o TIM_ERR_INVALID
 */
int tim_channel_output_enable(TIM_RegDef_t *TIMx, uint8_t channel, uint8_t enable);

/**
 * @brief  Habilita la salida principal del TIM1 (MOE bit en BDTR).
 *         Solo necesario para TIM1 (timer avanzado).
 * @param  TIMx: puntero al timer (debe ser TIM1)
 * @retval TIM_OK o TIM_ERR_INVALID
 */
int tim_bdtr_moe_enable(TIM_RegDef_t *TIMx);

#endif /* TIM_DRIVER_H */
