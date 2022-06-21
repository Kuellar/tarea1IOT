#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include <arpa/inet.h>
#include <string.h>

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "WIFI_client.h"
#include "nvs.h"

/** GLOBALS **/

// event group to contain status information
static EventGroupHandle_t wifi_event_group;

// retry tracker
static int s_retry_num = 0;

unsigned char payload[19227];

// event handler for wifi events
void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START){
        ESP_LOGI(WIFI_TAG, "Connecting to AP...");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED){
        if (s_retry_num < MAX_FAILURES){
            ESP_LOGI(WIFI_TAG, "Reconnecting to AP...");
            esp_wifi_connect();
            s_retry_num++;
        }
        else{
            xEventGroupSetBits(wifi_event_group, WIFI_FAILURE);
        }
    }
}

// event handler for ip events
void ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP){
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(WIFI_TAG, "STA IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_SUCCESS);
    }
}

// connect to wifi and return the result
esp_err_t connect_wifi(){
    int status = WIFI_FAILURE;

    // get SSID and PASS
    size_t required_size_ssid = 0;
    status = Read_NVS_str(NULL, 11, &required_size_ssid);
    if (status != 0) {
        return WIFI_FAILURE;
    }
    char* WIFI_SSID = malloc(required_size_ssid);
    status = Read_NVS_str(WIFI_SSID, 11, &required_size_ssid);
    if (status != 0) {
        return WIFI_FAILURE;
    }
    size_t required_size_pass = 0;
    status = Read_NVS_str(NULL, 12, &required_size_pass);
    if (status != 0) {
        return WIFI_FAILURE;
    }
    char* WIFI_PASS = malloc(required_size_pass);
    status = Read_NVS_str(WIFI_PASS, 12, &required_size_pass);
    if (status != 0) {
        return WIFI_FAILURE;
    }

    // FIX newline bug
    WIFI_SSID[strcspn(WIFI_SSID, "\n")] = '\0';
    WIFI_PASS[strcspn(WIFI_PASS, "\n")] = '\0';
    printf("WIFI SSID: %s.\n", WIFI_SSID);
    printf("WIFI PASS: %s.\n", WIFI_PASS);

    // initialize the esp network interface
    ESP_ERROR_CHECK(esp_netif_init());

    // initialize default esp event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // create wifi station in the wifi driver
    esp_netif_create_default_wifi_sta();

    // setup wifi station with the default wifi configuration
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // EVENT LOOP CRAZINESS
    wifi_event_group = xEventGroupCreate();

    esp_event_handler_instance_t wifi_handler_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        NULL,
        &wifi_handler_event_instance
    ));

    esp_event_handler_instance_t got_ip_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &ip_event_handler,
        NULL,
        &got_ip_event_instance
    ));

    // START THE WIFI DRIVER
    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };

    strcpy((char *)wifi_config.sta.ssid,(char *)WIFI_SSID);
    strcpy((char *)wifi_config.sta.password,(char *)WIFI_PASS);

    // set the wifi controller to be a station
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // set the wifi config
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // start the wifi driver
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(WIFI_TAG, "STA initialization complete");

    // NOW WE WAIT
    EventBits_t bits = xEventGroupWaitBits(
        wifi_event_group,
        WIFI_SUCCESS | WIFI_FAILURE,
        pdFALSE,
        pdFALSE,
        portMAX_DELAY
    );

    // xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually happened.
    if (bits & WIFI_SUCCESS){
        ESP_LOGI(WIFI_TAG, "Connected to ap");
        status = WIFI_SUCCESS;
    }
    else if (bits & WIFI_FAILURE){
        ESP_LOGI(WIFI_TAG, "Failed to connect to ap");
        status = WIFI_FAILURE;
    }
    else{
        ESP_LOGE(WIFI_TAG, "UNEXPECTED EVENT");
        status = WIFI_FAILURE;
    }

    // The event will not be processed after unregister
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, got_ip_event_instance));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handler_event_instance));
    vEventGroupDelete(wifi_event_group);

    return status;
}

esp_err_t connect_UDP_server(){
    struct sockaddr_in serverInfo = {0};
    char readBuffer[2] = {0};

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr.s_addr = UDP_HOST;
    serverInfo.sin_port = htons(UDP_PORT);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0){
        ESP_LOGE(WIFI_TAG, "Failed to create a socket..?");
        return UDP_FAILURE;
    }

    if (connect(sock, (struct sockaddr *)&serverInfo, sizeof(serverInfo)) != 0){
        ESP_LOGE(WIFI_TAG, "Failed to connect to %s!", inet_ntoa(serverInfo.sin_addr.s_addr));
        close(sock);
        return UDP_FAILURE;
    }

    ESP_LOGI(WIFI_TAG, "Connected to UDP server.");

    /*
    // CONECTADO A UDP
    bzero(readBuffer, sizeof(readBuffer));
    // PROTOCOL 4
    ESP_LOGI(WIFI_TAG, "Antes del for");
    for (;;)
    {
        bzero(payload, sizeof(payload));
        get_protocol_4(payload);
        ESP_LOGI(WIFI_TAG, "Antes de enviar");
        int err = send(sock, payload, sizeof(payload), 0);
        ESP_LOGI(WIFI_TAG, "Despues de enviar");
    }
    */

    return UDP_SUCCESS;
}

esp_err_t connect_TCP_server() {
    struct sockaddr_in serverInfo = {0};
    char readBuffer[2] = {0};

    serverInfo.sin_family = AF_INET;
    // serverInfo.sin_addr.s_addr = TCP_HOST;
    // serverInfo.sin_port = htons(TCP_PORT);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0){
        ESP_LOGE(WIFI_TAG, "Failed to create a socket..?");
        return TCP_FAILURE;
    }

    if (connect(sock, (struct sockaddr *)&serverInfo, sizeof(serverInfo)) != 0)
    {
        ESP_LOGE(WIFI_TAG, "Failed to connect to %s!", inet_ntoa(serverInfo.sin_addr.s_addr));
        close(sock);
        return TCP_FAILURE;
    }

    ESP_LOGI(WIFI_TAG, "Connected to TCP server.");

    // for (;;)
    // {
    //     // CONECTADO A TCP
    //     bzero(readBuffer, sizeof(readBuffer));
    //     unsigned char payload[55];

    //     bzero(payload, sizeof(payload));
    //     // PROTOCOL 0

    //     switch (protocolo_actual)
    //     {
    //     case 0:
    //         ESP_LOGI(TAG, "Protocolo 0");
    //         get_protocol_0(payload, 0, 6);
    //         protocolo_actual = 1;
    //         break;
    //     case 1:
    //         ESP_LOGI(TAG, "Protocolo 1");
    //         get_protocol_1(payload, 1, 16);
    //         protocolo_actual = 2;
    //         break;
    //     case 2:
    //         ESP_LOGI(TAG, "Protocolo 2");
    //         get_protocol_2(payload, 2, 20);
    //         protocolo_actual = 3;
    //         break;
    //     case 3:
    //         ESP_LOGI(TAG, "Protocolo 3");
    //         get_protocol_3(payload, 3, 44);
    //         protocolo_actual = 4;
    //         break;
    //     }

    //     int err = send(sock, payload, sizeof(payload), 0);
    //     int len = recv(sock, readBuffer, sizeof(readBuffer) - 1, 0);

    //     if (protocolo_actual == 4)
    //     {
    //         int vacio = send(sock, 0, 0, 0); // Corta la conexion
    //         break;
    //     }

    //     shutdown(sock, 0);
    //     close(sock);
    //     esp_deep_sleep(1e6 * 60);
    // }
    return TCP_SUCCESS;
}
