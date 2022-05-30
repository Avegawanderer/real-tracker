/**
    Project: Real Tracker
    Module:
    Author: avegawanderer@gmail.com
    Creation date:
    Description:

    WEB server

    See:
    https://github.com/Molorius/esp32-websocket
    https://github.com/Molorius/ESP32-Examples/tree/master/websocket_server
    https://randomnerdtutorials.com/esp32-web-server-websocket-sliders/
    https://github.com/lucadentella/esp32lights/blob/master/firmware/main/main.c
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


#include "websocket_server.h"


//----------- Definitions ----------//

//----------- Variables ------------//

static QueueHandle_t client_queue;
const static int client_queue_size = 10;

//----------- Externals ------------//

//----------- Prototypes -----------//

void webData_OnTextMessage(int clientNum, char *msg, int len);


//--------- Implementation ---------//



// handles websocket events
void websocket_callback(uint8_t num, WEBSOCKET_TYPE_t type, char *msg, uint64_t len)
{
    const static char *TAG = "websocket_callback";

    switch (type)
    {
        case WEBSOCKET_CONNECT:
            ESP_LOGI(TAG, "client %i connected!", num);
            break;
        case WEBSOCKET_DISCONNECT_EXTERNAL:
            ESP_LOGI(TAG, "client %i sent a disconnect message", num);
            //led_duty(0);
            break;
        case WEBSOCKET_DISCONNECT_INTERNAL:
            ESP_LOGI(TAG, "client %i was disconnected", num);
            break;
        case WEBSOCKET_DISCONNECT_ERROR:
            ESP_LOGI(TAG, "client %i was disconnected due to an error", num);
            //led_duty(0);
            break;
        case WEBSOCKET_TEXT:
            if (len)
            {
                ESP_LOGI(TAG, "Text message, len %u", (uint32_t)len);
                webData_OnTextMessage(num, msg, len);
            }
            break;
        case WEBSOCKET_BIN:
            ESP_LOGI(TAG, "client %i sent binary message of size %i:\n%s", num, (uint32_t )len, msg);
            break;
        case WEBSOCKET_PING:
            ESP_LOGI(TAG, "client %i pinged us with message of size %i:\n%s", num, (uint32_t )len, msg);
            break;
        case WEBSOCKET_PONG:
            ESP_LOGI(TAG, "client %i responded to the ping", num);
            break;
    }
}

// serves any clients
static void http_serve(struct netconn *conn)
{
    const static char *TAG = "http_server";
    const static char HTML_HEADER[] = "HTTP/1.1 200 OK\nContent-type: text/html\n\n";
    const static char ERROR_HEADER[] = "HTTP/1.1 404 Not Found\nContent-type: text/html\n\n";
    const static char JS_HEADER[] = "HTTP/1.1 200 OK\nContent-type: text/javascript\n\n";
    const static char CSS_HEADER[] = "HTTP/1.1 200 OK\nContent-type: text/css\n\n";
    //const static char PNG_HEADER[] = "HTTP/1.1 200 OK\nContent-type: image/png\n\n";
    //const static char ICO_HEADER[] = "HTTP/1.1 200 OK\nContent-type: image/x-icon\n\n";
    //const static char PDF_HEADER[] = "HTTP/1.1 200 OK\nContent-type: application/pdf\n\n";
    //const static char EVENT_HEADER[] = "HTTP/1.1 200 OK\nContent-Type: text/event-stream\nCache-Control: no-cache\nretry: 3000\n\n";
    struct netbuf *inbuf;
    static char *buf;
    static uint16_t buflen;
    static err_t err;

    // default page
    extern const uint8_t root_html_start[] asm("_binary_root_html_start");
    extern const uint8_t root_html_end[] asm("_binary_root_html_end");
    const uint32_t root_html_len = root_html_end - root_html_start;

    // error page
    extern const uint8_t error_html_start[] asm("_binary_error_html_start");
    extern const uint8_t error_html_end[] asm("_binary_error_html_end");
    const uint32_t error_html_len = error_html_end - error_html_start;

    // script.js
    extern const uint8_t script_js_start[] asm("_binary_script_js_start");
    extern const uint8_t script_js_end[] asm("_binary_script_js_end");
    const uint32_t script_js_len = script_js_end - script_js_start;

    // style.css
    extern const uint8_t style_css_start[] asm("_binary_style_css_start");
    extern const uint8_t style_css_end[] asm("_binary_style_css_end");
    const uint32_t style_css_len = style_css_end - style_css_start;

    netconn_set_recvtimeout(conn, 1000); // allow a connection timeout of 1 second
    ESP_LOGI(TAG, "reading from client...");
    err = netconn_recv(conn, &inbuf);
    ESP_LOGI(TAG, "read from client");
    if (err == ERR_OK)
    {
        netbuf_data(inbuf, (void**) &buf, &buflen);
        if (buf)
        {
            // default page
            if (strstr(buf, "GET / ") && !strstr(buf, "Upgrade: websocket"))
            {
                ESP_LOGI(TAG, "Sending /");
                netconn_write(conn, HTML_HEADER, sizeof(HTML_HEADER) - 1, NETCONN_NOCOPY);
                netconn_write(conn, root_html_start, root_html_len, NETCONN_NOCOPY);
                netconn_close(conn);
                netconn_delete(conn);
                netbuf_delete(inbuf);
            }

            // default page websocket
            else if (strstr(buf, "GET / ") && strstr(buf, "Upgrade: websocket"))
            {
                ESP_LOGI(TAG, "Requesting websocket on /");
                ws_server_add_client(conn, buf, buflen, "/", websocket_callback);
                netbuf_delete(inbuf);
            }

            else if (strstr(buf, "GET /script.js "))
            {
                ESP_LOGI(TAG, "Sending /script.js");
                netconn_write(conn, JS_HEADER, sizeof(JS_HEADER) - 1, NETCONN_NOCOPY);
                netconn_write(conn, script_js_start, script_js_len, NETCONN_NOCOPY);
                netconn_close(conn);
                netconn_delete(conn);
                netbuf_delete(inbuf);
            }

            else if (strstr(buf, "GET /style.css "))
            {
                ESP_LOGI(TAG, "Sending /style.css");
                netconn_write(conn, CSS_HEADER, sizeof(CSS_HEADER) - 1, NETCONN_NOCOPY);
                netconn_write(conn, style_css_start, style_css_len, NETCONN_NOCOPY);
                netconn_close(conn);
                netconn_delete(conn);
                netbuf_delete(inbuf);
            }

            else if (strstr(buf, "GET /"))
            {
                ESP_LOGI(TAG, "Unknown request, sending error page: %s", buf);
                netconn_write(conn, ERROR_HEADER, sizeof(ERROR_HEADER) - 1, NETCONN_NOCOPY);
                netconn_write(conn, error_html_start, error_html_len, NETCONN_NOCOPY);
                netconn_close(conn);
                netconn_delete(conn);
                netbuf_delete(inbuf);
            }

            else
            {
                ESP_LOGI(TAG, "Unknown request");
                netconn_close(conn);
                netconn_delete(conn);
                netbuf_delete(inbuf);
            }
        }
        else
        {
            ESP_LOGI(TAG, "Unknown request (empty?...)");
            netconn_close(conn);
            netconn_delete(conn);
            netbuf_delete(inbuf);
        }
    }
    else
    { // if err==ERR_OK
        ESP_LOGI(TAG, "error on read, closing connection");
        netconn_close(conn);
        netconn_delete(conn);
        netbuf_delete(inbuf);
    }
}



// handles clients when they first connect. passes to a queue
static void server_task(void *pvParameters)
{
    const static char *TAG = "server_task";
    struct netconn *conn, *newconn;
    static err_t err;
    client_queue = xQueueCreate(client_queue_size, sizeof(struct netconn*));

    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, NULL, 80);
    netconn_listen(conn);
    ESP_LOGI(TAG, "server listening");
    do
    {
        err = netconn_accept(conn, &newconn);
        ESP_LOGI(TAG, "new client");
        if (err == ERR_OK)
        {
            xQueueSendToBack(client_queue, &newconn, portMAX_DELAY);
            //http_serve(newconn);
        }
    } while (err == ERR_OK);
    netconn_close(conn);
    netconn_delete(conn);
    ESP_LOGE(TAG, "task ending, rebooting board");
    esp_restart();
}


// receives clients from queue, handles them
static void server_handle_task(void *pvParameters)
{
    const static char *TAG = "server_handle_task";
    struct netconn *conn;
    ESP_LOGI(TAG, "task starting");
    for (;;)
    {
        xQueueReceive(client_queue, &conn, portMAX_DELAY);
        if (!conn)
            continue;
        http_serve(conn);
    }
    vTaskDelete(NULL);
}

//
//static void count_task(void *pvParameters)
//{
//    const static char *TAG = "count_task";
//    char out[20];
//    int len;
//    int clients;
//    uint8_t n = 0;
//    const int DELAY = 1000 / portTICK_PERIOD_MS; // 1 second
//
//    ESP_LOGI(TAG, "starting task");
//    for (;;)
//    {
//        len = sprintf(out, "C%i", n);
//        clients = ws_server_send_text_all(out, len);
//        if (clients > 0)
//        {
//            //ESP_LOGI(TAG,"sent: \"%s\" to %i clients",out,clients);
//        }
//        n++;
//        vTaskDelay(DELAY);
//    }
//}




void webserver(void)
{
    ws_server_start();
    xTaskCreate(&server_task, "server_task", 3000, NULL, 9, NULL);
    xTaskCreate(&server_handle_task, "server_handle_task", 4000, NULL, 6, NULL);
    //xTaskCreate(&count_task, "count_task", 6000, NULL, 2, NULL);
}


