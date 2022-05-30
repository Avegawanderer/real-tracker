#ifndef __MAG_HMC5883_H__
#define __MAG_HMC5883_H__

#include "global.h"
#include "config.h"

#include "mag_sensor.h"


extern magSensor_t mag_hmc5883;


#ifdef __cplusplus
extern "C" {
#endif

    bool drvHmc5883_Detect(void);
    bool drvHmc5883_Init(void (*dataReadyCallback)());
    bool drvHmc5883_Read(int16_t *magData);

#ifdef __cplusplus
}   // extern "C"
#endif

#endif // __MAG_HMC5883_H__
