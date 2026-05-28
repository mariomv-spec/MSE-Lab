/**
 * @file    spi_driver.h
 * @brief   SPI Driver – CMSIS-Style Header
 *
 * @details Provee todas las definiciones de tipos, mapa de registros, códigos
 *          de estado y declaraciones de la API pública requeridas para
 *          inicializar, transmitir, recibir y controlar el chip-select del
 *          periférico SPI de un STM32F411xC/E.
 *
 *          El control del pin CS se delega completamente al driver GPIO ya
 *          existente (GPIO_stm32.h / GPIO_stm32.c) usando sus tipos y
 *          funciones públicas:
 *              - gpio_port_t / gpio_pin_t  →  identifican el pin CS
 *              - gpio_clearPin()           →  spi_csEnable()   (CS bajo)
 *              - gpio_setPin()             →  spi_csDisable()  (CS alto)
 *
 *          Por tanto este archivo NO redefine ni GPIO_RegDef_t, ni los
 *          base-addresses de GPIO, ni RCC_AHB1ENR_GPIOxEN; todos esos
 *          símbolos quedan a cargo de GPIO_stm32.h.
 *
 * @note    Referencias
 *          - STM32F411xC/E Reference Manual (RM0383), Rev 3.
 *          - STM32F411RE Datasheet (DS9716), Rev 4.
 *          - IEEE Std 830-1998: SRS Recommended Practice.
 *
 * @version 1.0
 * @author  Carlos Villarreal
 */

#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

/* =========================================================================
 * Dependencias
 *   - <stdint.h> / <stddef.h> : tipos primitivos del estándar C99.
 *   - GPIO_stm32.h            : gpio_port_t, gpio_pin_t, gpio_status_t,
 *                               gpio_setPin(), gpio_clearPin() y los macros
 *                               __IO/__I/__O.
 * ========================================================================= */

#include <stdint.h>
#include <stddef.h>
#include "GPIO_stm32.h"   /* gpio_port_t, gpio_pin_t, gpio_setPin/clearPin  */

/* =========================================================================
 * 1.  MACROS DE ACCESO A REGISTROS
 *     __IO / __I / __O ya están definidos en GPIO_stm32.h; se protegen
 *     para evitar redefinición en caso de inclusión en otro orden.
 * ========================================================================= */

#ifndef __IO
  #define __IO  volatile
#endif
#ifndef __I
  #define __I   volatile const
#endif
#ifndef __O
  #define __O   volatile
#endif

/** Conversión de dirección base a puntero de periférico SPI. */
#define SPI_PERIPH(base)   ((SPI_RegDef_t *)(base))

/* =========================================================================
 * 2.  DIRECCIONES BASE SPI  (STM32F411xC/E – RM0383 §2.3)
 * ========================================================================= */

/** @defgroup spi_base SPI Base Addresses
 *  @{
 */
#define SPI1_BASE_ADDR   (0x40013000UL)   /*!< SPI1 – APB2               */
#define SPI2_BASE_ADDR   (0x40003800UL)   /*!< SPI2 – APB1               */
#define SPI3_BASE_ADDR   (0x40003C00UL)   /*!< SPI3 – APB1               */
#define SPI4_BASE_ADDR   (0x40013400UL)   /*!< SPI4 – APB2               */
#define SPI5_BASE_ADDR   (0x40015000UL)   /*!< SPI5 – APB2 (F411 only)   */
/** @} */

/** Instancia por defecto si no se define externamente. */
#ifndef SPI_INSTANCE
  #define SPI_INSTANCE   SPI1_BASE_ADDR
#endif

/* =========================================================================
 * 3.  MAPA DE REGISTROS SPI  (RM0383 §20.5)
 * ========================================================================= */

/**
 * @brief  Estructura del mapa de registros SPI.
 *
 *         Los offsets de byte corresponden directamente a la Tabla 97 del
 *         RM0383. Todos los campos son @c volatile para evitar que el
 *         compilador optimice accesos a registros mapeados en memoria.
 */
typedef struct
{
    __IO uint32_t CR1;      /*!< 0x00 – Control Register 1          */
    __IO uint32_t CR2;      /*!< 0x04 – Control Register 2          */
    __IO uint32_t SR;       /*!< 0x08 – Status Register             */
    __IO uint32_t DR;       /*!< 0x0C – Data Register               */
    __IO uint32_t CRCPR;    /*!< 0x10 – CRC Polynomial Register     */
    __I  uint32_t RXCRCR;   /*!< 0x14 – RX CRC Register (solo lectura) */
    __I  uint32_t TXCRCR;   /*!< 0x18 – TX CRC Register (solo lectura) */
    __IO uint32_t I2SCFGR;  /*!< 0x1C – I2S Configuration Register  */
    __IO uint32_t I2SPR;    /*!< 0x20 – I2S Prescaler Register      */
} SPI_RegDef_t;

/* =========================================================================
 * 4.  BITS DEL REGISTRO CR1  (RM0383 §20.5.1)
 * ========================================================================= */

/** @defgroup spi_cr1 SPI CR1 Bit Masks
 *  @{
 */
#define SPI_CR1_CPHA_Pos      (0U)
#define SPI_CR1_CPHA_Msk      (0x1UL << SPI_CR1_CPHA_Pos)

#define SPI_CR1_CPOL_Pos      (1U)
#define SPI_CR1_CPOL_Msk      (0x1UL << SPI_CR1_CPOL_Pos)

#define SPI_CR1_MSTR_Pos      (2U)
#define SPI_CR1_MSTR_Msk      (0x1UL << SPI_CR1_MSTR_Pos)

#define SPI_CR1_BR_Pos        (3U)
#define SPI_CR1_BR_Msk        (0x7UL << SPI_CR1_BR_Pos)

#define SPI_CR1_SPE_Pos       (6U)
#define SPI_CR1_SPE_Msk       (0x1UL << SPI_CR1_SPE_Pos)

#define SPI_CR1_LSBFIRST_Pos  (7U)
#define SPI_CR1_LSBFIRST_Msk  (0x1UL << SPI_CR1_LSBFIRST_Pos)

#define SPI_CR1_SSI_Pos       (8U)
#define SPI_CR1_SSI_Msk       (0x1UL << SPI_CR1_SSI_Pos)

#define SPI_CR1_SSM_Pos       (9U)
#define SPI_CR1_SSM_Msk       (0x1UL << SPI_CR1_SSM_Pos)

#define SPI_CR1_DFF_Pos       (11U)
#define SPI_CR1_DFF_Msk       (0x1UL << SPI_CR1_DFF_Pos)

#define SPI_CR1_BIDIMODE_Pos  (15U)
#define SPI_CR1_BIDIMODE_Msk  (0x1UL << SPI_CR1_BIDIMODE_Pos)
/** @} */

/* =========================================================================
 * 5.  BITS DEL REGISTRO SR  (RM0383 §20.5.3)
 * ========================================================================= */

/** @defgroup spi_sr SPI SR Bit Masks
 *  @{
 */
#define SPI_SR_RXNE_Pos  (0U)
#define SPI_SR_RXNE_Msk  (0x1UL << SPI_SR_RXNE_Pos)  /*!< RX buffer Not Empty */

#define SPI_SR_TXE_Pos   (1U)
#define SPI_SR_TXE_Msk   (0x1UL << SPI_SR_TXE_Pos)   /*!< TX buffer Empty     */

#define SPI_SR_OVR_Pos   (6U)
#define SPI_SR_OVR_Msk   (0x1UL << SPI_SR_OVR_Pos)   /*!< Overrun flag        */

#define SPI_SR_BSY_Pos   (7U)
#define SPI_SR_BSY_Msk   (0x1UL << SPI_SR_BSY_Pos)   /*!< Busy flag           */
/** @} */

/* =========================================================================
 * 6.  RCC – habilitación del clock SPI  (FR-2)
 *     Solo se definen los bits de APBxENR para SPI; los bits GPIO ya
 *     están definidos en GPIO_stm32.h y no se repiten aquí.
 * ========================================================================= */

/**
 * @note  RCC_BASE y RCC_AHB1ENR ya están definidos en GPIO_stm32.h.
 *        Solo se agregan los registros APBxENR y sus bits SPI.
 */

/** APB1ENR – registro de habilitación de clock en APB1. */
#define RCC_APB1ENR   (*((__IO uint32_t *)(0x40023800UL + 0x40UL)))

/** APB2ENR – registro de habilitación de clock en APB2. */
#define RCC_APB2ENR   (*((__IO uint32_t *)(0x40023800UL + 0x44UL)))

/** @defgroup rcc_spi_en Bits de habilitación de clock SPI
 *  @{
 */
/* APB1ENR */
#define RCC_APB1ENR_SPI2EN_Pos  (14U)
#define RCC_APB1ENR_SPI2EN_Msk  (0x1UL << RCC_APB1ENR_SPI2EN_Pos)

#define RCC_APB1ENR_SPI3EN_Pos  (15U)
#define RCC_APB1ENR_SPI3EN_Msk  (0x1UL << RCC_APB1ENR_SPI3EN_Pos)

/* APB2ENR */
#define RCC_APB2ENR_SPI1EN_Pos  (12U)
#define RCC_APB2ENR_SPI1EN_Msk  (0x1UL << RCC_APB2ENR_SPI1EN_Pos)

#define RCC_APB2ENR_SPI4EN_Pos  (13U)
#define RCC_APB2ENR_SPI4EN_Msk  (0x1UL << RCC_APB2ENR_SPI4EN_Pos)

#define RCC_APB2ENR_SPI5EN_Pos  (20U)
#define RCC_APB2ENR_SPI5EN_Msk  (0x1UL << RCC_APB2ENR_SPI5EN_Pos)
/** @} */

/* =========================================================================
 * 7.  CONSTANTES DE CONFIGURACIÓN
 * ========================================================================= */

/**
 * @defgroup spi_baud Prescaler de baud rate (CR1[5:3])
 * @{
 */
#define SPI_BAUD_DIV2    (0x00UL << SPI_CR1_BR_Pos)  /*!< fPCLK / 2   */
#define SPI_BAUD_DIV4    (0x01UL << SPI_CR1_BR_Pos)  /*!< fPCLK / 4   */
#define SPI_BAUD_DIV8    (0x02UL << SPI_CR1_BR_Pos)  /*!< fPCLK / 8   */
#define SPI_BAUD_DIV16   (0x03UL << SPI_CR1_BR_Pos)  /*!< fPCLK / 16  */
#define SPI_BAUD_DIV32   (0x04UL << SPI_CR1_BR_Pos)  /*!< fPCLK / 32  */
#define SPI_BAUD_DIV64   (0x05UL << SPI_CR1_BR_Pos)  /*!< fPCLK / 64  */
#define SPI_BAUD_DIV128  (0x06UL << SPI_CR1_BR_Pos)  /*!< fPCLK / 128 */
#define SPI_BAUD_DIV256  (0x07UL << SPI_CR1_BR_Pos)  /*!< fPCLK / 256 */
/** @} */

/**
 * @defgroup spi_mode Modos SPI (combinaciones CPOL/CPHA)
 * @{
 */
#define SPI_MODE_0  (0UL)                                        /*!< CPOL=0, CPHA=0 */
#define SPI_MODE_1  (SPI_CR1_CPHA_Msk)                          /*!< CPOL=0, CPHA=1 */
#define SPI_MODE_2  (SPI_CR1_CPOL_Msk)                          /*!< CPOL=1, CPHA=0 */
#define SPI_MODE_3  (SPI_CR1_CPOL_Msk | SPI_CR1_CPHA_Msk)      /*!< CPOL=1, CPHA=1 */
/** @} */

/**
 * @defgroup spi_dff Tamaño del frame de datos
 * @{
 */
#define SPI_DFF_8BIT   (0UL)              /*!< Frame de  8 bits */
#define SPI_DFF_16BIT  (SPI_CR1_DFF_Msk)  /*!< Frame de 16 bits */
/** @} */

/**
 * @defgroup spi_bit_order Orden de bits
 * @{
 */
#define SPI_FIRSTBIT_MSB  (0UL)                   /*!< MSB primero */
#define SPI_FIRSTBIT_LSB  (SPI_CR1_LSBFIRST_Msk)  /*!< LSB primero */
/** @} */

/* =========================================================================
 * 8.  TIMEOUT
 * ========================================================================= */

/**
 * @brief  Número máximo de iteraciones de polling antes de retornar
 *         SPI_ERR_TIMEOUT.  Sobreescribible con -DSPI_TIMEOUT=<valor>.
 */
#ifndef SPI_TIMEOUT
  #define SPI_TIMEOUT   (100000UL)
#endif

/* =========================================================================
 * 9.  ESTRUCTURA DE CONFIGURACIÓN
 * ========================================================================= */

/**
 * @brief  Parámetros de inicialización del periférico SPI.
 *
 *         El llamador debe rellenar todos los campos antes de invocar
 *         spi_init(). Los valores válidos para cada campo se encuentran
 *         en los grupos de constantes SPI_BAUD_*, SPI_MODE_*, etc.
 */
typedef struct
{
    uint32_t baudRate;        /*!< Prescaler – uno de SPI_BAUD_DIVx     */
    uint32_t mode;            /*!< Modo SPI   – uno de SPI_MODE_x       */
    uint32_t dataFrameFormat; /*!< Tamaño de frame – SPI_DFF_8/16BIT    */
    uint32_t firstBit;        /*!< Orden de bits – SPI_FIRSTBIT_MSB/LSB */
} SPI_Config_t;

/* =========================================================================
 * 10. CÓDIGOS DE ESTADO / ERROR  (NFR-2)
 * ========================================================================= */

/**
 * @brief  Valores de retorno de todas las funciones públicas del driver SPI.
 *
 *         Se define como un tipo propio (SPI_Status_t) para no generar
 *         confusión con gpio_status_t del driver GPIO.
 */
typedef enum
{
    SPI_OK            = 0x00U, /*!< Operación completada con éxito              */
    SPI_ERR_NULL_PTR  = 0x01U, /*!< Puntero NULL pasado como argumento de buffer */
    SPI_ERR_BUSY      = 0x02U, /*!< Bus SPI ocupado                             */
    SPI_ERR_OVERRUN   = 0x03U, /*!< Error de overrun en el buffer RX            */
    SPI_ERR_TIMEOUT   = 0x04U, /*!< Timeout esperando un flag (TXE/RXNE/BSY)   */
    SPI_ERR_PARAM     = 0x05U, /*!< Parámetro inválido (p. ej. length == 0)     */
    SPI_ERR_GPIO      = 0x06U, /*!< El driver GPIO retornó error en operación CS */
} SPI_Status_t;

/* =========================================================================
 * 11. DESCRIPTOR DEL PIN CHIP-SELECT
 *
 *     Usa los tipos gpio_port_t / gpio_pin_t del driver GPIO existente.
 *     spi_csEnable() llama a gpio_clearPin() (CS activo-bajo, FR-7).
 *     spi_csDisable() llama a gpio_setPin()  (CS inactivo-alto, FR-8).
 *
 *     Pre-condición: el pin debe haber sido configurado como OUTPUT con
 *     gpio_setPinMode(port, pin, GPIO_MODE_OUTPUT) antes de cualquier
 *     llamada a spi_csEnable/Disable.
 * ========================================================================= */

/**
 * @brief  Descriptor del pin de chip-select.
 *
 *         Agrupa el puerto y el número de pin usando los tipos públicos del
 *         driver GPIO para que spi_csEnable() / spi_csDisable() puedan
 *         operar cualquier pin de cualquier puerto sin hard-coding.
 */
typedef struct
{
    gpio_port_t port;   /*!< Puerto GPIO – valor de gpio_port_t  */
    gpio_pin_t  pin;    /*!< Número de pin – valor de gpio_pin_t */
} SPI_CS_t;

/* =========================================================================
 * 12. DECLARACIONES DE LA API PÚBLICA
 * ========================================================================= */

/**
 * @defgroup spi_api API Pública del Driver SPI
 * @{
 */

/**
 * @brief  Inicializa el periférico SPI.
 *
 *         Configura el SPI en modo maestro según @p config y habilita el
 *         clock del periférico en el registro RCC_APBxENR (FR-1, FR-2).
 *
 *         La configuración de los pines SPI como función alterna debe
 *         realizarse antes con gpio_setPinMode() + gpio_setAlternateFunction()
 *         del driver GPIO (SRS §2.5).
 *
 * @param[in]  spi     Puntero al mapa de registros SPI destino.
 *                     Usar SPI_PERIPH(SPI1_BASE_ADDR), etc.
 * @param[in]  config  Puntero a estructura SPI_Config_t completamente
 *                     inicializada.
 *
 * @retval SPI_OK          Periférico inicializado correctamente.
 * @retval SPI_ERR_NULL_PTR  @p spi o @p config es NULL.
 * @retval SPI_ERR_PARAM   Algún campo de @p config contiene un valor inválido.
 */
SPI_Status_t spi_init(SPI_RegDef_t *spi, const SPI_Config_t *config);

/**
 * @brief  Transmite un buffer de bytes por el bus SPI.
 *
 *         Escribe @p length bytes desde @p pTxBuf en el registro DR,
 *         sondeando TXE antes de cada byte y BSY al finalizar (FR-3, FR-4).
 *
 * @param[in]  spi      Puntero al mapa de registros SPI.
 * @param[in]  buffer   Puntero al buffer fuente. No debe ser NULL y debe
 *                      contener al menos @p length bytes.
 * @param[in]  length   Número de bytes a transmitir. Debe ser > 0.
 *
 * @retval SPI_OK           Todos los bytes transmitidos.
 * @retval SPI_ERR_NULL_PTR @p spi o @p pTxBuf es NULL.
 * @retval SPI_ERR_PARAM    @p length es cero.
 * @retval SPI_ERR_TIMEOUT  TXE o BSY no se limpió dentro de SPI_TIMEOUT.
 */
SPI_Status_t spi_transmit(SPI_RegDef_t *spi, const uint8_t *buffer, size_t length);

/**
 * @brief  Recibe un buffer de bytes por el bus SPI.
 *
 *         Escribe un byte dummy para generar el clock SPI y lee @p length
 *         bytes del registro DR en @p pRxBuf, sondeando RXNE antes de cada
 *         lectura (FR-5, FR-6).
 *
 * @param[in]  spi      Puntero al mapa de registros SPI.
 * @param[out] buffer   Puntero al buffer destino. No debe ser NULL y debe
 *                      tener capacidad para al menos @p length bytes.
 * @param[in]  length   Número de bytes a recibir. Debe ser > 0.
 *
 * @retval SPI_OK           Todos los bytes recibidos.
 * @retval SPI_ERR_NULL_PTR @p spi o @p pRxBuf es NULL.
 * @retval SPI_ERR_PARAM    @p length es cero.
 * @retval SPI_ERR_OVERRUN  Flag OVR detectado durante la recepción.
 * @retval SPI_ERR_TIMEOUT  RXNE no se activó dentro de SPI_TIMEOUT.
 */
SPI_Status_t spi_receive(SPI_RegDef_t *spi, uint8_t *buffer, size_t length);

/**
 * @brief  Activa (pone en LOW) el pin chip-select.
 *
 *         Internamente llama a gpio_clearPin(cs->port, cs->pin) del driver
 *         GPIO para seleccionar el dispositivo SPI destino (FR-7).
 *
 * @param[in]  cs  Puntero a descriptor SPI_CS_t completamente inicializado.
 *                 El pin debe estar previamente configurado como OUTPUT
 *                 mediante gpio_setPinMode().
 *
 * @retval SPI_OK         Pin CS puesto en LOW.
 * @retval SPI_ERR_NULL_PTR  @p cs es NULL.
 * @retval SPI_ERR_GPIO   gpio_clearPin() retornó un código de error.
 */
SPI_Status_t spi_csEnable(const SPI_CS_t *cs);

/**
 * @brief  Desactiva (pone en HIGH) el pin chip-select.
 *
 *         Internamente llama a gpio_setPin(cs->port, cs->pin) del driver
 *         GPIO para deseleccionar el dispositivo SPI (FR-8).
 *
 * @param[in]  cs  Puntero a descriptor SPI_CS_t completamente inicializado.
 *
 * @retval SPI_OK         Pin CS puesto en HIGH.
 * @retval SPI_ERR_NULL_PTR  @p cs es NULL.
 * @retval SPI_ERR_GPIO   gpio_setPin() retornó un código de error.
 */
SPI_Status_t spi_csDisable(const SPI_CS_t *cs);

/** @} */ /* fin de spi_api */

/* =========================================================================
 * 13. EJEMPLO DE USO  (SRS §6)
 *
 * @code
 *
 *  #include "GPIO_stm32.h"
 *  #include "spi_driver.h"
 *
 *  // --- 1. Configurar pines SPI y CS con el driver GPIO ------------------
 *  void board_spiPinsInit(void)
 *  {
 *      // Habilitar clock de GPIOA
 *      gpio_initPort(GPIO_PORT_A);
 *
 *      // PA5=SCK, PA6=MISO, PA7=MOSI → Función alterna AF5 (SPI1)
 *      gpio_setPinMode(GPIO_PORT_A, GPIO_PIN_5, GPIO_MODE_ALT_FUNC);
 *      gpio_setAlternateFunction(GPIO_PORT_A, GPIO_PIN_5, GPIO_AF5);
 *
 *      gpio_setPinMode(GPIO_PORT_A, GPIO_PIN_6, GPIO_MODE_ALT_FUNC);
 *      gpio_setAlternateFunction(GPIO_PORT_A, GPIO_PIN_6, GPIO_AF5);
 *
 *      gpio_setPinMode(GPIO_PORT_A, GPIO_PIN_7, GPIO_MODE_ALT_FUNC);
 *      gpio_setAlternateFunction(GPIO_PORT_A, GPIO_PIN_7, GPIO_AF5);
 *
 *      // PA4=CS → salida digital, se pone HIGH (inactivo) por defecto
 *      gpio_setPinMode(GPIO_PORT_A, GPIO_PIN_4, GPIO_MODE_OUTPUT);
 *      gpio_setPin(GPIO_PORT_A, GPIO_PIN_4);
 *  }
 *
 *  // --- 2. Descriptor del pin CS -----------------------------------------
 *  static const SPI_CS_t g_cs = {
 *      .port = GPIO_PORT_A,
 *      .pin  = GPIO_PIN_4
 *  };
 *
 *  // --- 3. Configuración del periférico SPI --------------------------------
 *  static const SPI_Config_t g_spiCfg = {
 *      .baudRate        = SPI_BAUD_DIV16,
 *      .mode            = SPI_MODE_0,
 *      .dataFrameFormat = SPI_DFF_8BIT,
 *      .firstBit        = SPI_FIRSTBIT_MSB
 *  };
 *
 *  void app_spiExample(void)
 *  {
 *      SPI_RegDef_t *spi = SPI_PERIPH(SPI1_BASE_ADDR);
 *      SPI_Status_t  st;
 *
 *      board_spiPinsInit();
 *
 *      // Inicializar periférico SPI
 *      st = spi_init(spi, &g_spiCfg);
 *      if (st != SPI_OK) { return; }
 *
 *      uint8_t txData[3] = { 0x9F, 0x00, 0x00 };  // p.ej. comando JEDEC-ID
 *      uint8_t rxData[3] = { 0 };
 *
 *      // Secuencia CS → TX → RX → CS
 *      spi_csEnable(&g_cs);                        // CS bajo  (gpio_clearPin)
 *
 *      st = spi_transmit(spi, txData, sizeof(txData));
 *      if (st != SPI_OK) { spi_csDisable(&g_cs); return; }
 *
 *      st = spi_receive(spi, rxData, sizeof(rxData));
 *
 *      spi_csDisable(&g_cs);                       // CS alto  (gpio_setPin)
 *
 *      (void)rxData;   // procesar rxData[] según el protocolo del dispositivo
 *  }
 *
 * @endcode
 * ========================================================================= */

#endif /* SPI_DRIVER_H */