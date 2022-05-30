/**
    Project: Real Tracker
    Module:
    Author: avegawanderer@gmail.com
    Creation date:
    Description:

    Implementation of networking functions
*/


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

//----------- Definitions ----------//

//----------- Variables ------------//

static const char *TAG = "WiFi softAP";



const uint8_t myIp[4] = IP_ADDR_MY;
const uint8_t gwIp[4] = IP_ADDR_GW;
const uint8_t netMask[4] = NET_MASK;

//----------- Externals ------------//

//----------- Prototypes -----------//

//--------- Implementation ---------//




static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d", MAC2STR(event->mac), event->aid);
    }
}


void setupNetwork(void)
{
    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");

    esp_netif_t *netif = esp_netif_create_default_wifi_ap();

    // Setting static IP
    // See also
    //https://github.com/qienhuang/esp32_mini_webserver_gpio/blob/master/main/hello_world_main.c

    ESP_ERROR_CHECK(esp_netif_dhcps_stop(netif));
    esp_netif_ip_info_t ip_info;

    IP4_ADDR(&ip_info.ip, myIp[0], myIp[1], myIp[2], myIp[3]);
    IP4_ADDR(&ip_info.gw, gwIp[0], gwIp[1], gwIp[2], gwIp[3]);
    IP4_ADDR(&ip_info.netmask, netMask[0], netMask[1], netMask[2], netMask[3]);
    ESP_ERROR_CHECK(esp_netif_set_ip_info(netif, &ip_info));
#if ENA_DHCP == 1
    ESP_ERROR_CHECK(esp_netif_dhcps_start(netif));              // If sending UDP socket is bound to INADDR_ANY and DHCP is disabled,
                                                                // sending broadcast packets to 255.255.255.255 fails with EHOSTUNREACH (errno 118)
#endif

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                            ESP_EVENT_ANY_ID,
                                                            &wifi_event_handler,
                                                            NULL,
                                                            NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = ESP_WIFI_SSID,
            .ssid_len = strlen(ESP_WIFI_SSID),
            .channel = ESP_WIFI_CHANNEL,
            .password = ESP_WIFI_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             ESP_WIFI_SSID, ESP_WIFI_PASS, ESP_WIFI_CHANNEL);
}


