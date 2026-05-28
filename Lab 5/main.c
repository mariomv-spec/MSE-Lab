/**
 * @file    main.c
 * @brief   Sistema de Control RGBC (AEC/AGC) — máquina de estados principal.
 *
 * Flujo por presión de botón:
 *   IDLE → READ_SENSOR → WAIT_RELEASE → IDLE
 *
 * read_sensor_data() llama a rgbc_control_update() una sola vez.
 * El bucle de convergencia (Fase 1 + Fase 2) vive dentro del módulo de
 * control; esta función retorna solo cuando el sistema es estable o falla.
 */

#include "GPIO_stm32.h"
#include "uart_driver.h"
#include "serial.h"
#include "i2c_driver.h"
#include "tcs3472_driver.h"
 
/* --- Prototipos locales --- */
static void hardware_init(void);
static void read_and_print(void);
 
typedef enum {
    IDLE         = 0,
    READ_SENSOR  = 1,
    WAIT_RELEASE = 2,
} state_t;
 
int main(void)
{
    hardware_init();
 
    state_t state        = IDLE;
    uint8_t button_state = 0;
 
    serial_printf("\r\n=== TCS3472 — Lectura RGBC ===\r\n");
    serial_printf("Presiona el boton para leer el sensor.\r\n\r\n");
 
    while (1)
    {
        switch (state)
        {
            case IDLE:
                gpio_readPin(GPIO_PORT_C, GPIO_PIN_13, &button_state);
                if (button_state == 0)
                    state = READ_SENSOR;
                break;
 
            case READ_SENSOR:
                read_and_print();
                state = WAIT_RELEASE;
                break;
 
            case WAIT_RELEASE:
                gpio_readPin(GPIO_PORT_C, GPIO_PIN_13, &button_state);
                if (button_state == 1)
                    state = IDLE;
                break;
 
            default:
                state = IDLE;
                break;
        }
    }
 
    return 0;
}
 
/**
 * @brief  Lee los cuatro canales RGBC y los imprime en terminal.
 */
static void read_and_print(void)
{
    tcs3472_rgbc_t   data;
    tcs3472_status_t st = tcs3472_read_rgbc(I2C1, &data);
 
    switch (st)
    {
        case TCS3472_OK:
            serial_printf("C: %u  R: %u  G: %u  B: %u\r\n",
                          (unsigned int)data.clear,
                          (unsigned int)data.red,
                          (unsigned int)data.green,
                          (unsigned int)data.blue);
            break;
 
        case TCS3472_ERR_SATURATED:
            serial_printf("!!! Sensor saturado (Clear = %u). "
                          "Reduce la ganancia o el tiempo de integracion.\r\n",
                          (unsigned int)data.clear);
            break;
 
        case TCS3472_ERR_TIMEOUT:
            serial_printf("[!] Timeout esperando AVALID.\r\n");
            break;
 
        default:
            serial_printf("[!] Error I2C. Codigo: %d\r\n", (int)st);
            break;
    }
}
 
/**
 * @brief  Configura pines y periféricos.
 */
static void hardware_init(void)
{
    /* Habilitar FPU */
    uint32_t *SCB_CPACR = (uint32_t *)0xE000ED88;
    *SCB_CPACR |= (0xF << 20);
 
    /* UART2 Setup */
    gpio_initPort(GPIO_PORT_A);
    gpio_setPinMode(GPIO_PORT_A, GPIO_PIN_2, GPIO_MODE_ALT_FUNC);
    gpio_setAlternateFunction(GPIO_PORT_A, GPIO_PIN_2, 7);
    serial_init();
 
    /* Botón de usuario — PC13, entrada */
    gpio_initPort(GPIO_PORT_C);
    gpio_setPinMode(GPIO_PORT_C, GPIO_PIN_13, GPIO_MODE_INPUT);
 
    /* I2C1 Setup */
    gpio_initPort(GPIO_PORT_B);
    gpio_setPinMode(GPIO_PORT_B, GPIO_PIN_8, GPIO_MODE_ALT_FUNC);
    gpio_setPinMode(GPIO_PORT_B, GPIO_PIN_9, GPIO_MODE_ALT_FUNC);
    gpio_setAlternateFunction(GPIO_PORT_B, GPIO_PIN_8, 4);
    gpio_setAlternateFunction(GPIO_PORT_B, GPIO_PIN_9, 4);
    gpio_setOpenDrain(GPIO_PORT_B, GPIO_PIN_8);
    gpio_setOpenDrain(GPIO_PORT_B, GPIO_PIN_9);
    gpio_setPullUp(GPIO_PORT_B, GPIO_PIN_8);
    gpio_setPullUp(GPIO_PORT_B, GPIO_PIN_9);
 
    i2c_config_t i2c_cfg = {
        .instance  = I2C1,
        .clk_speed = I2C_SPEED_SM_HZ,
        .addr_mode = I2C_ADDR_7BIT,
    };
    i2c_init(&i2c_cfg);
 
    /* TCS3472 — ganancia 4x, integración 154 ms */
    tcs3472_config_t sensor_cfg = {
        .i2c   = I2C1,
        .gain  = TCS3472_GAIN_4X,
        .atime = TCS3472_ATIME_154MS,
    };
 
    tcs3472_status_t st = tcs3472_init(&sensor_cfg);
 
    if (st != TCS3472_OK)
    {
        serial_printf("!!! Error al inicializar el sensor. Codigo: %d\r\n", (int)st);
        while (1);  /* Error de hardware */
    }
 
    serial_printf("+++ Sensor TCS3472 inicializado. GAIN: 4x | ATIME: 154ms\r\n");
}
 