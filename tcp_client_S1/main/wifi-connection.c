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

/** PROTOCOL **/
#define BYTE sizeof(char)
#define ID_DEVICE_SPACE 2
#define MAC_SPACE 6
#define ID_PROTOCOL_SPACE 1
#define LENG_MSG_SPACE 2
#define DATA_1_SPACE 1
#define DATA_2_SPACE 1
#define DATA_3_SPACE 4
#define DATA_4_SPACE 1
#define DATA_5_SPACE 4
#define DATA_6_SPACE 1
#define DATA_N_SPACE 4

/** GLOBALS **/

// event group to contain status information
static EventGroupHandle_t wifi_event_group;

// retry tracker
static int s_retry_num = 0;

// task tag
static const char *TAG = "WIFI";
/** FUNCTIONS **/

void get_protocol_0(unsigned char *arr, unsigned char prot, unsigned int lmesg)
{
    unsigned int device = 9;
    unsigned long long int mac = 68465;
    unsigned char protocol = prot;
    unsigned int leng_mesg = lmesg;
    unsigned int data_1 = 1;
    unsigned int data_2 = 50;
    time_t data_3 = time(NULL);

    int i = 0;
    memcpy(arr, &device, BYTE * ID_DEVICE_SPACE);
    i += ID_DEVICE_SPACE;
    memcpy(&(arr[i]), &mac, BYTE * MAC_SPACE);
    i += MAC_SPACE;
    memcpy(&(arr[i]), &protocol, BYTE * ID_PROTOCOL_SPACE);
    i += ID_PROTOCOL_SPACE;
    memcpy(&(arr[i]), &leng_mesg, BYTE * LENG_MSG_SPACE);
    i += LENG_MSG_SPACE;
    memcpy(&(arr[i]), &data_1, BYTE * DATA_1_SPACE);
    i += DATA_1_SPACE;
    memcpy(&(arr[i]), &data_2, BYTE * DATA_2_SPACE);
    i += DATA_2_SPACE;
    memcpy(&(arr[i]), &data_3, BYTE * DATA_3_SPACE);
}

void get_protocol_1(unsigned char *arr, unsigned char prot, unsigned int lmesg)
{
    get_protocol_0(arr, prot, lmesg);

    unsigned int data_4 = 123;
    unsigned int data_5 = 12345;
    unsigned int data_6 = 123;
    unsigned int data_7 = 12345;

    int i = 17;
    memcpy(&(arr[i]), &data_4, BYTE * DATA_4_SPACE);
    i += DATA_4_SPACE;
    memcpy(&(arr[i]), &data_5, BYTE * DATA_5_SPACE);
    i += DATA_5_SPACE;
    memcpy(&(arr[i]), &data_6, BYTE * DATA_6_SPACE);
    i += DATA_6_SPACE;
    memcpy(&(arr[i]), &data_7, BYTE * DATA_N_SPACE);
}

void get_protocol_2(unsigned char *arr, unsigned char prot, unsigned int lmesg)
{
    get_protocol_1(arr, prot, lmesg);

    unsigned int data_8 = 12345;

    int i = 27;
    memcpy(&(arr[i]), &data_8, BYTE * DATA_N_SPACE);
}

void get_protocol_3(unsigned char *arr, unsigned char prot, unsigned int lmesg)
{
    get_protocol_2(arr, prot, lmesg);

    unsigned int data_9 = 1234;
    unsigned int data_10 = 5678;
    unsigned int data_11 = 9123;
    unsigned int data_12 = 4567;
    unsigned int data_13 = 8901;
    unsigned int data_14 = 2345;

    int i = 31;
    memcpy(&(arr[i]), &data_9, BYTE * DATA_N_SPACE);
    i += DATA_N_SPACE;
    memcpy(&(arr[i]), &data_10, BYTE * DATA_N_SPACE);
    i += DATA_N_SPACE;
    memcpy(&(arr[i]), &data_11, BYTE * DATA_N_SPACE);
    i += DATA_N_SPACE;
    memcpy(&(arr[i]), &data_12, BYTE * DATA_N_SPACE);
    i += DATA_N_SPACE;
    memcpy(&(arr[i]), &data_13, BYTE * DATA_N_SPACE);
    i += DATA_N_SPACE;
    memcpy(&(arr[i]), &data_14, BYTE * DATA_N_SPACE);
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
    char readBuffer[2] = {0};

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
    unsigned char payload[55];

    // PROTOCOL 0
    bzero(payload, sizeof(payload));
    get_protocol_0(payload, 0, 6);

    int err = send(sock, payload, sizeof(payload), 0);
    int len = recv(sock, readBuffer, sizeof(readBuffer) - 1, 0);

    // DEEP SLEEP
    //  PROTOCOL 1
    bzero(readBuffer, sizeof(readBuffer));
    bzero(payload, sizeof(payload));
    get_protocol_1(payload, 1, 16);

    int err = send(sock, payload, sizeof(payload), 0);
    int len = recv(sock, readBuffer, sizeof(readBuffer) - 1, 0);

    // DEEP SLEEP
    //  PROTOCOL 2
    bzero(readBuffer, sizeof(readBuffer));
    bzero(payload, sizeof(payload));
    get_protocol_1(payload, 2, 20);

    int err = send(sock, payload, sizeof(payload), 0);
    int len = recv(sock, readBuffer, sizeof(readBuffer) - 1, 0);

    // DEEP SLEEP
    //  PROTOCOL 3
    bzero(readBuffer, sizeof(readBuffer));
    bzero(payload, sizeof(payload));
    get_protocol_1(payload, 3, 44);

    int err = send(sock, payload, sizeof(payload), 0);
    int len = recv(sock, readBuffer, sizeof(readBuffer) - 1, 0);

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