/**
    Project: Real Tracker
    Module:
    Author: avegawanderer@gmail.com
    Creation date:
    Description:

    Header file for PWM servo driver
*/

#ifndef __DRV_PWM_H__
#define __DRV_PWM_H__

#include "global.h"

#ifdef __cplusplus
extern "C" {
#endif

    void drvPwm_Init(void);
    void drvPwm_SetPanDuty(uint16_t us);
    void drvPwm_SetTiltDuty(uint16_t us);


#ifdef __cplusplus
}   // extern "C"
#endif

#endif // __DRV_PWM_H__
