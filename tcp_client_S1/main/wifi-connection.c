#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

/** DEFINES **/
#define WIFI_SUCCESS 1 << 0
#define WIFI_FAILURE 1 << 1
#define TCP_SUCCESS 1 << 0
#define TCP_FAILURE 1 << 1
#define MAX_FAILURES 10

/** GLOBALS **/

// event group to contain status information
static EventGroupHandle_t wifi_event_group;

// retry tracker
static int s_retry_num = 0;

// task tag
static const char *TAG = "WIFI";
/** FUNCTIONS **/

void split(int val, unsigned char *arr)
{
    memcpy(arr, &val, sizeof(int));
}

// event handler for wifi events
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(TAG, "Connecting to AP...");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < MAX_FAILURES)
        {
            ESP_LOGI(TAG, "Reconnecting to AP...");
            esp_wifi_connect();
            s_retry_num++;
        }
        else
        {
            xEventGroupSetBits(wifi_event_group, WIFI_FAILURE);
        }
    }
}

// event handler for ip events
static void ip_event_handler(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data)
{
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "STA IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_SUCCESS);
    }
}

// connect to wifi and return the result
esp_err_t connect_wifi()
{
    int status = WIFI_FAILURE;

    /** INITIALIZE ALL THE THINGS **/
    // initialize the esp network interface
    ESP_ERROR_CHECK(esp_netif_init());

    // initialize default esp event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // create wifi station in the wifi driver
    esp_netif_create_default_wifi_sta();

    // setup wifi station with the default wifi configuration
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /** EVENT LOOP CRAZINESS **/
    wifi_event_group = xEventGroupCreate();

    esp_event_handler_instance_t wifi_handler_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &wifi_handler_event_instance));

    esp_event_handler_instance_t got_ip_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &ip_event_handler,
                                                        NULL,
                                                        &got_ip_event_instance));

    /** START THE WIFI DRIVER **/
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "Lucas",        // ipulsera
            .password = "asrs7mik", // holaaa11
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false},
        },
    };

    // set the wifi controller to be a station
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // set the wifi config
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // start the wifi driver
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "STA initialization complete");

    /** NOW WE WAIT **/
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
                                           WIFI_SUCCESS | WIFI_FAILURE,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_SUCCESS)
    {
        ESP_LOGI(TAG, "Connected to ap");
        status = WIFI_SUCCESS;
    }
    else if (bits & WIFI_FAILURE)
    {
        ESP_LOGI(TAG, "Failed to connect to ap");
        status = WIFI_FAILURE;
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
        status = WIFI_FAILURE;
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, got_ip_event_instance));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handler_event_instance));
    vEventGroupDelete(wifi_event_group);

    return status;
}

// connect to the server and return the result

esp_err_t connect_tcp_server(void)
{
    struct sockaddr_in serverInfo = {0};
    char readBuffer[55] = {0};

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr.s_addr = 0x0801a8c0;
    serverInfo.sin_port = htons(5001);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        ESP_LOGE(TAG, "Failed to create a socket..?");
        return TCP_FAILURE;
    }

    if (connect(sock, (struct sockaddr *)&serverInfo, sizeof(serverInfo)) != 0)
    {
        ESP_LOGE(TAG, "Failed to connect to %s!", inet_ntoa(serverInfo.sin_addr.s_addr));
        close(sock);
        return TCP_FAILURE;
    }

    ESP_LOGI(TAG, "Connected to TCP server.");

    // CONECTADO A TCP

    bzero(readBuffer, sizeof(readBuffer));

    // int8_t originalArray[13] = {0, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    // unsigned int n = 5000;

    // unsigned int payload[55];
    //  n = (char *)n; //"5000"
    //  payload[0] = (n >> 8) & 0xFF;
    //  payload[1] = (n >> 0) & 0xFF;
    char payload[55];

    unsigned char device[2];
    unsigned char mac[6];
    unsigned char protocol[1];
    split(15, device);
    split(68465, mac);
    split(0, protocol);
    // int n = 123;
    //  char *a = (char *)&n;
    // payload[0] = (char *)25565;
    // payload[2] = (char *)123;
    // payload[8] = (char *)17;

    for (int i = 0; i < 2; i++)
        payload[i + 0] = device[i];
    for (int i = 0; i < 6; i++)
        payload[i + 2] = mac[i];
    for (int i = 0; i < 1; i++)
        payload[i + 8] = protocol[i];
    /**
    payload[0] = 25565;
    payload[2] = 300; // MAC
    payload[4] = 300;
    payload[6] = 300;
    payload[8] = 0;    // ID Protocol
    payload[9] = 300;  // Msg len
    payload[11] = 1;   // value
    payload[12] = 50;  // battery
    payload[13] = 105; // timestamp
    */

    // static const char *payload = "Message from ESP32 ";
    int err = send(sock, payload, sizeof(payload), 0);

    int len = recv(sock, readBuffer, sizeof(readBuffer) - 1, 0);

    ESP_LOGI(TAG, "HOLA");
    /**
    int r = read(sock, readBuffer, sizeof(readBuffer) - 1);
    for (int i = 0; i < r; i++)
    {
        putchar(readBuffer[i]);
    }

    if (strcmp(readBuffer, "HELLO") == 0)
    {
        ESP_LOGI(TAG, "WE DID IT!");
    }
    */
    return TCP_SUCCESS;
}

void app_main(void)
{
    esp_err_t status = WIFI_FAILURE;

    // initialize storage
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // connect to wireless AP
    status = connect_wifi();
    if (WIFI_SUCCESS != status)
    {
        ESP_LOGI(TAG, "Failed to associate to AP, dying...");
        return;
    }
    status = connect_tcp_server();
    if (TCP_SUCCESS != status)
    {
        ESP_LOGI(TAG, "Failed to connect to remote server, dying...");
        return;
    }
}