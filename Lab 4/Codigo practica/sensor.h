#ifndef SENSOR_H
#define SENSOR_H


#include "adc_driver.h"
#include <stdint.h>

typedef enum
{
    SENSOR_OK = 0,
    SENSOR_ERR
} 
sensor_status_t;

sensor_status_t sensor_init(Adc_Instance_t instance, Adc_Channel_t channel);
sensor_status_t sensor_startConversion(Adc_Instance_t instance);
sensor_status_t sensor_readValue(Adc_Instance_t instance, uint32_t *value);

#endif /* SENSOR_H */