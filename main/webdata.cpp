/**
    Project: Real Tracker
    Module:
    Author: avegawanderer@gmail.com
    Creation date:
    Description:

    Intermidiate layer between WEB server and the rest of the system
*/

#include <string.h>
#include "esp_log.h"

#include "websocket_server.h"
#include "system.h"
#include "cJSON.h"
#include "webdata.h"

#include "mag.h"




#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"

#include "lwip/netdb.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

#include "config.h"
#include "network.h"


#include "websocket_server.h"

//----------- Definitions ----------//

//----------- Variables ------------//

//static const char *TAG = "WebData";

//----------- Externals ------------//

//----------- Prototypes -----------//


//--------- Implementation ---------//


void webData_OnTextMessage(int clientNum, char *msg, int len)
{
    char *strVal;
    const char *respStr;
    const char *param;
    cJSON *requ_root = cJSON_Parse(msg);
    cJSON *answ_root = cJSON_CreateObject();
    sysMsg_t sysMsg;

    param = "pan_state";
    if (cJSON_GetObjectItem(requ_root, param))
    {
        strVal = cJSON_GetObjectItem(requ_root, param)->valuestring;
        if ((strVal) && (strVal[0] == '?'))
        {
            respStr = (System::stCalibratingMag ? "Calibrating MAG" : (
                      (System::stCalibratingPan) ? "Calibrating pan" : (
                      (System::stManualPanControl) ? "Manual" : "Tracking")));
            cJSON_AddStringToObject(answ_root, param, respStr);
        }
    }

    param = "pan_offset";
    if (cJSON_GetObjectItem(requ_root, param))
    {
        strVal = cJSON_GetObjectItem(requ_root, param)->valuestring;
        if ((strVal) && (strVal[0] == '?'))
        {
            cJSON_AddNumberToObject(answ_root, param, (int)Mag::getOffset());   // TODO - from Pan controller
        }
        else
        {
            Mag::setOffset(cJSON_GetObjectItem(requ_root, param)->valueint);    // TODO - by message through queue
        }
    }

    param = "pan_manual_mode";
    if (cJSON_GetObjectItem(requ_root, param))
    {
        strVal = cJSON_GetObjectItem(requ_root, param)->valuestring;
        if ((strVal) && (strVal[0] == '?'))
        {
            cJSON_AddNumberToObject(answ_root, param, System::stManualPanControl);
        }
        else
        {
            sysMsg.type = sysMsg_SetPanManualMode;
            sysMsg.commonRequestData.value1 = cJSON_GetObjectItem(requ_root, param)->valueint;
            xQueueSend(System::msgQueue, &sysMsg, portMAX_DELAY);
        }
    }

    param = "pan_manual_heading";
    if (cJSON_GetObjectItem(requ_root, param))
    {
        strVal = cJSON_GetObjectItem(requ_root, param)->valuestring;
        if ((strVal) && (strVal[0] == '?'))
        {
            cJSON_AddNumberToObject(answ_root, param, System::panManualHeading);
        }
        else
        {
            sysMsg.type = sysMsg_SetPanManualTarget;
            sysMsg.commonRequestData.value1 = cJSON_GetObjectItem(requ_root, param)->valueint;
            xQueueSend(System::msgQueue, &sysMsg, portMAX_DELAY);
        }
    }

    param = "pan_servo_inverted";
    if (cJSON_GetObjectItem(requ_root, param))
    {
        strVal = cJSON_GetObjectItem(requ_root, param)->valuestring;
        if ((strVal) && (strVal[0] == '?'))
        {
            cJSON_AddNumberToObject(answ_root, param, (int)0);
        }
        else
            {} // Set pan servo inversion
    }

    param = "pan_servo_low_start";
    if (cJSON_GetObjectItem(requ_root, param))
    {
        strVal = cJSON_GetObjectItem(requ_root, param)->valuestring;
        if ((strVal) && (strVal[0] == '?'))
        {
            cJSON_AddNumberToObject(answ_root, param, (int)1450);
        }
        else
        {
            // Set pan servo low start
        } 
    }

    param = "pan_servo_high_start";
    if (cJSON_GetObjectItem(requ_root, param))
    {
        strVal = cJSON_GetObjectItem(requ_root, param)->valuestring;
        if ((strVal) && (strVal[0] == '?'))
        {
            cJSON_AddNumberToObject(answ_root, param, (int)1550);
        }
        else
        {
            // Set pan servo high start
        } 
    }

    param = "pan_servo_calibrate";
    if (cJSON_GetObjectItem(requ_root, param))
    {
        sysMsg.type = sysMsg_CalibratePanServo;
        xQueueSend(System::msgQueue, &sysMsg, portMAX_DELAY);
    }

    param = "mag_calibrate";
    if (cJSON_GetObjectItem(requ_root, param))
    {
        sysMsg.type = sysMsg_CalibrateMag;
        xQueueSend(System::msgQueue, &sysMsg, portMAX_DELAY);
    }

    char *json_string;
    json_string = cJSON_Print(answ_root);
    len = strlen(json_string);
    ws_server_send_text_client_from_callback(clientNum, json_string, (uint64_t)len);
    cJSON_free(json_string);

    cJSON_Delete(requ_root);
    cJSON_Delete(answ_root);
}



void webData_RegularUpdateTask(void *pvParameters)
{
    // Broadcast runtime data
    cJSON *root;
    char *json_string;
    int len;
    uint32_t frameCounter = 0;
    const int DELAY = 500 / portTICK_PERIOD_MS;
    int clients;
    while (1)
    {
        root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, "frame", frameCounter++);
        cJSON_AddNumberToObject(root, "heading", (int)Mag::getHeading());
        cJSON_AddNumberToObject(root, "pan_us", 1500);          // TODO - obtain from system
        cJSON_AddNumberToObject(root, "tilt_us", 1500);        // TODO - obtain from system
        json_string = cJSON_Print(root);
        len = strlen(json_string);
        clients = ws_server_send_text_all(json_string, len);
        if (clients > 0)
        {
            //ESP_LOGI("Regular","sent: \"%s\" to %i clients", json_string, clients);
        }
        cJSON_free(json_string);
        cJSON_Delete(root);

        vTaskDelay(DELAY);
    }
}


void webdata(void)
{


    xTaskCreate(&webData_RegularUpdateTask, "update_task", 6000, NULL, 9, NULL);
}


