/**
    Project: Real Tracker
    Module:
    Author: avegawanderer@gmail.com
    Creation date:
    Description:

    Common definitions and function prototypes for sensors
    Partially adopted from https://github.com/raul-ortega/u360gts
*/

#ifndef __SENSORS_COMMON_H__
#define __SENSORS_COMMON_H__

#include "global.h"
#include "config.h"

typedef enum {
    ALIGN_DEFAULT,
    CW0_DEG,
    CW90_DEG,
    CW180_DEG,
    CW270_DEG,
    CW0_DEG_FLIP,
    CW90_DEG_FLIP,
    CW180_DEG_FLIP,
    CW270_DEG_FLIP
} SensorAlignment_t;


#ifdef __cplusplus
extern "C" {
#endif

    void alignSensor(const int16_t *src, int16_t *dest, SensorAlignment_t alignment);
    float rad2degf(float rad);

#ifdef __cplusplus
}   // extern "C"
#endif

#endif // __SENSORS_COMMON_H__
