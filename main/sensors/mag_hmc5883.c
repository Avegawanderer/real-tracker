/** 
    @file
    @brief   LED GPIO driver
    @author  borozdin.a
    @date    21.07.2020
*/

#include <math.h>
#include <stdlib.h>

#include "maths.h"
#include "drv_i2c.h"

#include "mag_hmc5883.h"

//------------ Definitions ----------//


#define MAG_ADDRESS             0x1E
#define MAG_DATA_REGISTER       0x03

#define HMC58X3_R_CONFA         0
#define HMC58X3_R_CONFB         1
#define HMC58X3_R_MODE          2

#define HMC58X3_X_SELF_TEST_GAUSS (+1.16f)       // X axis level when bias current is applied.
#define HMC58X3_Y_SELF_TEST_GAUSS (+1.16f)       // Y axis level when bias current is applied.
#define HMC58X3_Z_SELF_TEST_GAUSS (+1.08f)       // Z axis level when bias current is applied.

#define HMC_POS_BIAS 1
#define HMC_NEG_BIAS 2

//------------ Variables ------------//

static float magGain[3] = { 1.0f, 1.0f, 1.0f };

magSensor_t mag_hmc5883 = {
    drvHmc5883_Detect,
    drvHmc5883_Init,
    drvHmc5883_Read
};

//------------ Externals ------------//

//------------ Prototypes -----------//

//--------- Implementation ----------//


/**
    @brief Check if HMC5883 is present on I2C bus
        I2C driver must be enabled prior to to calling this function

    @param  None
    @return true if HMC5883 is present on the bus, false otherwise
*/
bool drvHmc5883_Detect(void)
{
    bool res;
    uint8_t sig = 0;

    res = drvI2C_ReadRegister(MAG_ADDRESS, 0x0A, &sig, 1);
    if ((res == false) || (sig != 'H'))
        return false;

    return true;
}


/**
    @brief Init HMC5883 driver
        I2C driver must be enabled prior to to calling this function

    @param  None
    @return true if success, false otherwise
*/
bool drvHmc5883_Init(void (*dataReadyCallback)())
{
    int16_t magADC[3];
    int i;
    // 32 bit totals so they won't overflow.
    int32_t xyz_total[3] = { 0, 0, 0 };
    bool bret = true;

    // Reset gain in case Init() is called more than once
    magGain[X] = 1.0f;
    magGain[Y] = 1.0f;
    magGain[Z] = 1.0f;
#if 0
    // Reg A DOR = 0x010 + MS1, MS0 set to pos bias
    drvI2C_WriteRegister(MAG_ADDRESS, HMC58X3_R_CONFA, 0x010 + HMC_POS_BIAS);

    // Set the Gain to 2.5Ga (7:5->011)
    // Note that the  very first measurement after a gain change maintains the same gain as the previous setting.
    // The new gain setting is effective from the second measurement and on.
    drvI2C_WriteRegister(MAG_ADDRESS, HMC58X3_R_CONFB, 0x60);

    vTaskDelay(100 / portTICK_PERIOD_MS);

    drvHmc5883_Read(magADC);

    // Collect 10 samples
    for (i = 0; i < 10; i++)
    {
        drvI2C_WriteRegister(MAG_ADDRESS, HMC58X3_R_MODE, 1);

        vTaskDelay(50 / portTICK_PERIOD_MS);

        drvHmc5883_Read(magADC);       // Get the raw values in case the scales have already been changed.

        // Since the measurements are noisy, they should be averaged rather than taking the max.
        xyz_total[X] += magADC[X];
        xyz_total[Y] += magADC[Y];
        xyz_total[Z] += magADC[Z];

        // Detect saturation.
        if (MIN(magADC[X], MIN(magADC[Y], magADC[Z])) <= -4096)
        {
            bret = false;
            break;              // Breaks out of the for loop.  No sense in continuing if we saturated.
        }
    }

    // Apply the negative bias. (Same gain)

    // Reg A DOR = 0x010 + MS1, MS0 set to negative bias.
    drvI2C_WriteRegister(MAG_ADDRESS, HMC58X3_R_CONFA, 0x010 + HMC_NEG_BIAS);

    // Collect 10 samples
    for (i = 0; i < 10; i++)
    {
        drvI2C_WriteRegister(MAG_ADDRESS, HMC58X3_R_MODE, 1);

        vTaskDelay(50 / portTICK_PERIOD_MS);

        drvHmc5883_Read(magADC);               // Get the raw values in case the scales have already been changed.

        // Since the measurements are noisy, they should be averaged.
        xyz_total[X] -= magADC[X];
        xyz_total[Y] -= magADC[Y];
        xyz_total[Z] -= magADC[Z];

        // Detect saturation.
        if (MIN(magADC[X], MIN(magADC[Y], magADC[Z])) <= -4096)
        {
            bret = false;
            break;              // Breaks out of the for loop.  No sense in continuing if we saturated.
        }
    }

    magGain[X] = fabsf(660.0f * HMC58X3_X_SELF_TEST_GAUSS * 2.0f * 10.0f / xyz_total[X]);
    magGain[Y] = fabsf(660.0f * HMC58X3_Y_SELF_TEST_GAUSS * 2.0f * 10.0f / xyz_total[Y]);
    magGain[Z] = fabsf(660.0f * HMC58X3_Z_SELF_TEST_GAUSS * 2.0f * 10.0f / xyz_total[Z]);
#endif
    // leave test mode
    drvI2C_WriteRegister(MAG_ADDRESS, HMC58X3_R_CONFA, 0x70);   // Configuration Register A  -- 0 11 100 00  num samples: 8 ; output rate: 15Hz ; normal measurement mode
    drvI2C_WriteRegister(MAG_ADDRESS, HMC58X3_R_CONFB, 0x20);   // Configuration Register B  -- 001 00000    configuration gain 1.3Ga
    drvI2C_WriteRegister(MAG_ADDRESS, HMC58X3_R_MODE, 0x00);    // Mode register             -- 000000 00    continuous Conversion Mode

    vTaskDelay(100 / portTICK_PERIOD_MS);

    if (!bret)
    {
        // Something went wrong so get a best guess
        magGain[X] = 1.0f;
        magGain[Y] = 1.0f;
        magGain[Z] = 1.0f;
    }

    //hmc5883lConfigureDataReadyInterruptHandling();
    // TODO: set callback for data ready



    return bret;
}


bool drvHmc5883_Read(int16_t *magData)
{
    uint8_t buf[6];

    bool ack = drvI2C_ReadRegister(MAG_ADDRESS, MAG_DATA_REGISTER, buf, 6);
    if (!ack)
    {
        return false;
    }

    // During calibration, magGain is 1.0, so the read returns normal non-calibrated values.
    // After calibration is done, magGain is set to calculated gain values.
    magData[X] = (int16_t)(buf[0] << 8 | buf[1]) * magGain[X];
    magData[Z] = (int16_t)(buf[2] << 8 | buf[3]) * magGain[Z];
    magData[Y] = (int16_t)(buf[4] << 8 | buf[5]) * magGain[Y];

    return true;
}

