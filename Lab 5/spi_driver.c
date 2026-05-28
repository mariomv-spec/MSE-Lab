#include "spi_driver.h"

SPI_Status_t spi_init(SPI_RegDef_t *spi, const SPI_Config_t *config){

    /* Verificación de punteros no nulos */
    if( spi == NULL || config == NULL ){
        return SPI_ERR_NULL_PTR;
    }
    
    /* Verificación de configuración válida */

    // Verificación de baudRate dentro de los valores 
    // permitidos por el hardware usando un mask de 4 bits 
    if( (config->baudRate & ~SPI_CR1_BR_Msk) != 0U ){
        return SPI_ERR_PARAM;
    }

    // Verificación de modo por medio de mask de bits correspondientes
    if( (config->mode & ~(SPI_CR1_CPOL_Msk | SPI_CR1_CPHA_Msk)) != 0U ){
        return SPI_ERR_PARAM;
    }

    // Verificación de dataFrameFormat por medio de mask de bits correspondiente
    if( (config->dataFrameFormat & ~SPI_CR1_DFF_Msk) != 0U ){
        return SPI_ERR_PARAM;
    }

    // Verificación de firstBit por medio de mask de bits correspondiente
    if( (config->firstBit & ~SPI_CR1_LSBFIRST_Msk) != 0U ){
        return SPI_ERR_PARAM;
    }

    /* Habilitar clock del periférico SPI según instancia */
    if( spi == SPI_PERIPH(SPI1_BASE_ADDR) ){
        RCC_APB2ENR |= RCC_APB2ENR_SPI1EN_Msk;
    } else if( spi == SPI_PERIPH(SPI2_BASE_ADDR) ){
        RCC_APB1ENR |= RCC_APB1ENR_SPI2EN_Msk;
    } else if( spi == SPI_PERIPH(SPI3_BASE_ADDR) ){
        RCC_APB1ENR |= RCC_APB1ENR_SPI3EN_Msk;
    } else if( spi == SPI_PERIPH(SPI4_BASE_ADDR) ){
        RCC_APB2ENR |= RCC_APB2ENR_SPI4EN_Msk;
    } else if( spi == SPI_PERIPH(SPI5_BASE_ADDR) ){
        RCC_APB2ENR |= RCC_APB2ENR_SPI5EN_Msk;
    } else {
        return SPI_ERR_PARAM;
}

    /* Se asume que la configuración de GPIO ya está realizada */

    /* Configuración del periférico SPI */

    // Limpieza del registro
    spi->CR1 = 0U;

    // Baud rate, modo, tamaño de frame y orden de bits
    spi->CR1 |= (config->baudRate & SPI_CR1_BR_Msk);
    spi->CR1 |= (config->mode & (SPI_CR1_CPOL_Msk | SPI_CR1_CPHA_Msk));
    spi->CR1 |= (config->dataFrameFormat & SPI_CR1_DFF_Msk);
    spi->CR1 |= (config->firstBit & SPI_CR1_LSBFIRST_Msk);

    // Modo maestro
    spi->CR1 |= SPI_CR1_MSTR_Msk;

    // Software slave management 
    spi->CR1 |= SPI_CR1_SSM_Msk | SPI_CR1_SSI_Msk;

    // Habilitar periférico SPI
    spi->CR1 |= SPI_CR1_SPE_Msk;

    return SPI_OK;
}


SPI_Status_t spi_transmit(SPI_RegDef_t *spi, const uint8_t *buffer, size_t length){

    /* Verificación de punteros no nulos */
    if( spi == NULL || buffer == NULL ){
        return SPI_ERR_NULL_PTR;
    }

    /* Verificación de longitud válida */
    if( length == 0U ){
        return SPI_ERR_PARAM;
    }

    /* Transmisión de bytes */

    for(size_t i = 0; i < length; i++){

        // Esperar a que el buffer de transmisión esté vacío
        size_t timeout = SPI_TIMEOUT;
        while( (spi->SR & SPI_SR_TXE_Msk) == 0U ){
            if( --timeout == 0U ){
                return SPI_ERR_TIMEOUT;
            }
        }

        // Escribir el byte a transmitir en el registro DR
        spi->DR = buffer[i];
    }

    /* Esperar a que la transmisión se complete */
    size_t timeout = SPI_TIMEOUT;
    while( (spi->SR & SPI_SR_BSY_Msk) != 0U ){
        if( --timeout == 0U ){
            return SPI_ERR_TIMEOUT;
        }
    }

    // Limpieza de OVR
    (void)spi->SR;
    (void)spi->DR;

    return SPI_OK;
}


SPI_Status_t spi_receive(SPI_RegDef_t *spi, uint8_t *buffer, size_t length){    

    /* Verificación de punteros no nulos */
    if( spi == NULL || buffer == NULL ){
        return SPI_ERR_NULL_PTR;
    }

    /* Verificación de longitud válida */
    if( length == 0U ){
        return SPI_ERR_PARAM;
    }

    /* Recepción de bytes */
    for(size_t i = 0; i < length; i++){

        // Esperar TXE antes de escribir el dummy byte
        size_t timeout = SPI_TIMEOUT;
        while( (spi->SR & SPI_SR_TXE_Msk) == 0U ){
            if( --timeout == 0U ){
                return SPI_ERR_TIMEOUT;
            }
        }

        // Enviar dummy byte para generar el clock y desplazar datos del esclavo
        spi->DR = 0x00U;

        // Esperar a que el byte recibido esté listo (RXNE)
        timeout = SPI_TIMEOUT;
        while( (spi->SR & SPI_SR_RXNE_Msk) == 0U ){
            if( --timeout == 0U ){
                return SPI_ERR_TIMEOUT;
            }
        }

        // Verificar overrun antes de leer
        if( (spi->SR & SPI_SR_OVR_Msk) != 0U ){
            (void)spi->DR;
            (void)spi->SR;
            return SPI_ERR_OVERRUN;
        }

        // Leer el byte recibido desde el registro DR
        buffer[i] = (uint8_t)spi->DR;
    }

    /* Esperar a que el periférico termine de desplazar el último byte */
    size_t timeout = SPI_TIMEOUT;
    while( (spi->SR & SPI_SR_BSY_Msk) != 0U ){
        if( --timeout == 0U ){
            return SPI_ERR_TIMEOUT;
        }
    }


    /* Limpiar OVR residual */
    (void)spi->SR;
    (void)spi->DR;

    return SPI_OK;
}


SPI_Status_t spi_csEnable(const SPI_CS_t *cs){
    
    /* Verificación de puntero no nulo */
    if( cs == NULL ){
        return SPI_ERR_NULL_PTR;
    }
    
    /* Llamada a gpio_clearPin() para poner el pin CS en LOW */
    gpio_status_t gpio_status = gpio_clearPin(cs->port, cs->pin);

    if (gpio_status != GPIO_OK) {
        return SPI_ERR_GPIO;
    }

    return SPI_OK;

}


SPI_Status_t spi_csDisable(const SPI_CS_t *cs){

    /* Verificación de puntero no nulo */
    if( cs == NULL ){
        return SPI_ERR_NULL_PTR;
    }
    
    /* Llamada a gpio_setPin() para poner el pin CS en HIGH */
    gpio_status_t gpio_status = gpio_setPin(cs->port, cs->pin);

    if (gpio_status != GPIO_OK) {
        return SPI_ERR_GPIO;
    }

    return SPI_OK;

}