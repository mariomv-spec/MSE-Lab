

#include "sensor.h"
#include "adc_driver.h"

sensor_status_t sensor_init(Adc_Instance_t instance, Adc_Channel_t channel) {
    
    // Se comprueba la inicialización en estado por defecto del ADC
    if (adc_init() != ADC_OK) return SENSOR_ERR;
    
    // Se habilita el ADC y se verifica que se haya hecho correctamente.
    if (adc_enableAdc(instance) != ADC_OK) return SENSOR_ERR;

    // Se configura el canal del ADC para la lectura del sensor.
    if (adc_setChannel(instance, channel) != ADC_OK) return SENSOR_ERR;
    
    return SENSOR_OK;
}

sensor_status_t sensor_startConversion(Adc_Instance_t instance) {
    
    // Se ordena al ADC que inicie una sola conversión de datos.
    return (adc_startSingleConversion(instance) == ADC_OK) ? SENSOR_OK : SENSOR_ERR;
}

sensor_status_t sensor_readValue(Adc_Instance_t instance, uint32_t *value) {

    // Se verifica que el puntero destino para guardar el dato no sea nulo.
    if (value == (void*)0) return SENSOR_ERR;
    
    uint16_t reg_val = 0;

    // Si la lectura del dato se realiza correctamente, se guarda
    // el valor de 16 bits como un dato de 32 bits en la variable destino.
    if (adc_readData(instance, &reg_val) == ADC_OK) {
        *value = (uint32_t)reg_val;
        return SENSOR_OK;
    }
    
    return SENSOR_ERR;
}