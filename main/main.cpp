/**
    Project: Real Tracker
    Module:
    Author: avegawanderer@gmail.com
    Creation date:
    Description:

    Implementation of main()
*/

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"

#include "global.h"
#include "config.h"

#include "drv_led.h"
#include "drv_pwm.h"
#include "drv_i2c.h"

#include "network.h"
#include "mag.h"
#include "system.h"
#include "webserver.h"
#include "webdata.h"

//----------- Definitions ----------//

//----------- Variables ------------//

//----------- Externals ------------//

//----------- Prototypes -----------//

extern "C" void app_main(void);

//--------- Implementation ---------//


void app_main(void)
{
    esp_err_t err;
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // See https://www.lucadentella.it/en/2018/01/08/esp32lights/
    nvs_handle my_handle;
    nvs_open("storage", NVS_READWRITE, &my_handle);
    //err = nvs_get_i32(my_handle, "mode", &working_mode);

    drvLed_Init();
    drvPwm_Init();
    drvI2C_Init();

    setupNetwork();

    xTaskCreate(System::Run, "System task", 4096, 0, 4, NULL);
    webserver();
    webdata();

    while(1)
    {
        vTaskDelay(TICK_PERIOD_MS / portTICK_PERIOD_MS);
        System::Tick();
    }
}


