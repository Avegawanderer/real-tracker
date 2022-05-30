/** 
    @file
    @brief   LED GPIO driver
    @author  borozdin.a
    @date    21.07.2020
*/

#include "driver/i2c.h"

#include "global.h"
#include "config.h"
#include "drv_i2c.h"


//------------ Definitions ----------//

#define I2C_MASTER_PORT             0           /*!< I2C master i2c port number */
#define I2C_MASTER_FREQ_HZ          100000      /*!< I2C master clock frequency */

#define I2C_MASTER_TIMEOUT_MS       100

//------------ Variables ------------//

//------------ Externals ------------//

//------------ Prototypes -----------//

//--------- Implementation ----------//


/**
    @brief Init I2C module


    @param  None
    @return true if success, false otherwise
*/
bool drvI2C_Init(void)
{
    esp_err_t espErr;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .clk_flags = 0,
    };
    i2c_param_config(I2C_MASTER_PORT, &conf);
    espErr = i2c_driver_install(I2C_MASTER_PORT, I2C_MODE_MASTER, 0, 0, 0);
    if (espErr == ESP_OK)
        return true;
    else
        return false;
}


/**
    @brief Read a sequence of bytes starting from a given register address within I2C device

    @param dev_addr address of the device
    @param reg_addr address of the register within the device to start with
    @param data pointer to data array to hold read data
    @param len number of bytes to read
    @return true if success, false otherwise
*/
bool drvI2C_ReadRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint32_t len)
{
    esp_err_t espErr = i2c_master_write_read_device(I2C_MASTER_PORT, dev_addr, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (espErr == ESP_OK)
        return true;
    else
        return false;
}


/**
    @brief Write a single byte into a register within I2C device

    @param dev_addr address of the device
    @param reg_addr address of the register within the device to start with
    @param data value to write
    @return true if success, false otherwise
*/
bool drvI2C_WriteRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t data)
{
    esp_err_t espErr;
    uint8_t write_buf[2] = {reg_addr, data};
    espErr = i2c_master_write_to_device(I2C_MASTER_PORT, dev_addr, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (espErr == ESP_OK)
        return true;
    else
        return false;
}







