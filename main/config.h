/**
    Project: Real Tracker
    Module:
    Author: avegawanderer@gmail.com
    Creation date:
    Description:

    Configuration file
*/
#ifndef __CONFIG_H__
#define __CONFIG_H__

// Include some library modules to enable syntax highlighting
#include "driver/gpio.h"
#include "driver/uart.h"


#define ESP_WIFI_SSID      "esp32_wifi"
#define ESP_WIFI_PASS      "11112222"
#define ESP_WIFI_CHANNEL   1
#define MAX_STA_CONN       5

#define IP_ADDR_MY                  {192, 168, 1, 10}
#define IP_ADDR_GW                  {192, 168, 1, 1}
#define NET_MASK                    {255, 255, 255, 0}
#define ENA_DHCP                    1

// TODO - rename according to functions
#define TELEM_LED_PIN               GPIO_NUM_2      // Blinks when telemetry data (any) is coming from telemetry UART
#define AAT_TELEM_MODE_LED_PIN      GPIO_NUM_27     // Active when AAT UART is in telemetry mode (receives telemetry)
#define AAT_CONFIG_MODE_LED_PIN     GPIO_NUM_25     // Active when AAT UART is in configurator mode (data exchange with UDP CONFIG_PORT)

#define I2C_MASTER_SCL_IO           GPIO_NUM_0      // gpio number for I2C master clock
#define I2C_MASTER_SDA_IO           GPIO_NUM_4      // gpio number for I2C master data

#define PAN_SERVO_PULSE_GPIO        GPIO_NUM_18
#define TILT_SERVO_PULSE_GPIO       GPIO_NUM_19


#endif  // __CONFIG_H__
