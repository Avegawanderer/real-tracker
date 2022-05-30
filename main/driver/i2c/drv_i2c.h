#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__

#include "global.h"

#ifdef __cplusplus
extern "C" {
#endif

    bool drvI2C_Init(void);
    bool drvI2C_ReadRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint32_t len);
    bool drvI2C_WriteRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);

#ifdef __cplusplus
}   // extern "C"
#endif

#endif // __DRV_I2C_H__
