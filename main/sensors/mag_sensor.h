/**
    Project: Real Tracker
    Module:
    Author: avegawanderer@gmail.com
    Creation date:
    Description:

    Declaration of magnetometer driver interface
*/

#ifndef __MAG_SENSOR_H__
#define __MAG_SENSOR_H__

#include "global.h"



typedef struct {
     bool (* detect)(void);
     bool (* init)(void (*dataReadyCallback)());
     bool (* read)(int16_t *data);
} magSensor_t;


#endif // __MAG_SENSOR_H__
