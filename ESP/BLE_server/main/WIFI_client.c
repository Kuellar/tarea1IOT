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

#include "nvs.h"
#include "WIFI_client.h"
#include "data.h"

/** GLOBALS **/

// event group to contain status information
static EventGroupHandle_t wifi_event_group;

// retry tracker
static int s_retry_num = 0;

uint8_t* tx_data;

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
    WIFI_PASS[required_size_pass-1] = '\0';
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
        ESP_LOGI(WIFI_TAG, "Connected to WIFI");
        status = WIFI_SUCCESS;
    }
    else if (bits & WIFI_FAILURE){
        ESP_LOGI(WIFI_TAG, "Failed to connect to WIFI");
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

esp_err_t connect_UDP_server(int32_t status){
    struct sockaddr_in serverInfo = {0};

    int32_t UDP_PORT, protocol, new_status, new_protocol;
    esp_err_t err_read_status;
    int err;

    err_read_status = Read_NVS(&UDP_PORT, 9);
    if (err_read_status == ESP_ERR_NVS_NOT_FOUND) {
        Write_NVS(0, 1);  // STATUS 0
        Write_NVS(0, 2);  // PROTOCOL 0
        return UDP_FAILURE;
    }

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr.s_addr = HOST;
    serverInfo.sin_port = (uint16_t) UDP_PORT;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0){
        ESP_LOGE(WIFI_TAG, "Failed to create a socket..?");
        return UDP_FAILURE;
    }

    if (connect(sock, (struct sockaddr *)&serverInfo, sizeof(serverInfo)) != 0){
        ESP_LOGE(WIFI_TAG, "Failed to connect to %s!", inet_ntoa(serverInfo.sin_addr.s_addr));
        ESP_LOGE(WIFI_TAG, "PORT %d!", serverInfo.sin_port);
        close(sock);
        return UDP_FAILURE;
    }

    ESP_LOGI(WIFI_TAG, "Connected to UDP server.");
    ESP_LOGI(WIFI_TAG, "ADDR %s!", inet_ntoa(serverInfo.sin_addr.s_addr));
    ESP_LOGI(WIFI_TAG, "PORT %d!", serverInfo.sin_port);

    // CONECTADO A UDP
    char readBuffer[2] = {0};  // NEW STATUS - PROTOCOL
    tx_data = (uint8_t*) malloc(sizeof(Sensor_Data_5));
    Sensor_Data_1 data1;
    Sensor_Data_2 data2;
    Sensor_Data_3 data3;
    Sensor_Data_4 data4;
    Sensor_Data_5 data5;
    for (;;){
        bzero(readBuffer, sizeof(readBuffer));
        // SEND DATA
        Read_NVS(&protocol, 2);
        switch (protocol){
        case 1:
            data1 = get_protocol_1((int8_t) status);
            memcpy(tx_data, &data1, sizeof(Sensor_Data_1));
            err = send(sock, tx_data, sizeof(Sensor_Data_1), 0);
            break;
        case 2:
            data2 = get_protocol_2((int8_t) status);
            memcpy(tx_data, &data2, sizeof(Sensor_Data_2));
            err = send(sock, tx_data, sizeof(Sensor_Data_2), 0);
            break;
        case 3:
            data3 = get_protocol_3((int8_t) status);
            memcpy(tx_data, &data3, sizeof(Sensor_Data_3));
            err = send(sock, tx_data, sizeof(Sensor_Data_3), 0);
            break;
        case 4:
            data4 = get_protocol_4((int8_t) status);
            memcpy(tx_data, &data4, sizeof(Sensor_Data_4));
            err = send(sock, tx_data, sizeof(Sensor_Data_4), 0);
            break;
        case 5:
            data5 = get_protocol_5((int8_t) status);
            memcpy(tx_data, &data5, sizeof(Sensor_Data_5));
            err = send(sock, tx_data, sizeof(Sensor_Data_5), 0);
            break;
        default:
            break;
        }

        // RECEIVE DATA
        int len = recv(sock, readBuffer, sizeof(readBuffer), 0);
        new_status = (int32_t) readBuffer[0];
        new_protocol = (int32_t) readBuffer[1];

        // CHANGE PROTOCOL
        if (protocol != new_protocol) {
            Write_NVS(new_protocol, 2);
        }

        // CHANGE STATUS
        if (status != new_status) {
            Write_NVS(new_status, 1);
            return UDP_SUCCESS;
        }
    }

    return UDP_SUCCESS;
}

esp_err_t connect_TCP_server(int32_t status) {
    struct sockaddr_in serverInfo = {0};

    int32_t TCP_PORT, protocol, new_status, new_protocol;
    esp_err_t err_read_status;
    int err ;

    err_read_status = Read_NVS(&TCP_PORT, 8);
    if (err_read_status == ESP_ERR_NVS_NOT_FOUND) {
        Write_NVS(0, 1);  // STATUS 0
        Write_NVS(0, 2);  // PROTOCOL 0
        return TCP_FAILURE;
    }

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr.s_addr = HOST;
    serverInfo.sin_port = (uint16_t) TCP_PORT;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0){
        ESP_LOGE(WIFI_TAG, "Failed to create a socket..?");
        return TCP_FAILURE;
    }

    if (connect(sock, (struct sockaddr *)&serverInfo, sizeof(serverInfo)) != 0)
    {
        ESP_LOGE(WIFI_TAG, "Failed to connect to %s!", inet_ntoa(serverInfo.sin_addr.s_addr));
        ESP_LOGE(WIFI_TAG, "PORT %d!", serverInfo.sin_port);
        close(sock);
        return TCP_FAILURE;
    }

    ESP_LOGI(WIFI_TAG, "Connected to TCP server.");
    ESP_LOGI(WIFI_TAG, "ADDR %s!", inet_ntoa(serverInfo.sin_addr.s_addr));
    ESP_LOGI(WIFI_TAG, "PORT %d!", serverInfo.sin_port);

    // CONECTADO A TCP
    char readBuffer[2] = {0};  // NEW STATUS - PROTOCOL
    tx_data = (uint8_t*) malloc(sizeof(Sensor_Data_5));
    Sensor_Data_1 data1;
    Sensor_Data_2 data2;
    Sensor_Data_3 data3;
    Sensor_Data_4 data4;
    Sensor_Data_5 data5;
    for (;;){
        bzero(readBuffer, sizeof(readBuffer));
        // SEND DATA
        Read_NVS(&protocol, 2);
        switch (protocol){
        case 1:
            data1 = get_protocol_1((int8_t) status);
            memcpy(tx_data, &data1, sizeof(Sensor_Data_1));
            err = send(sock, tx_data, sizeof(Sensor_Data_1), 0);
            break;
        case 2:
            data2 = get_protocol_2((int8_t) status);
            memcpy(tx_data, &data2, sizeof(Sensor_Data_2));
            err = send(sock, tx_data, sizeof(Sensor_Data_2), 0);
            break;
        case 3:
            data3 = get_protocol_3((int8_t) status);
            memcpy(tx_data, &data3, sizeof(Sensor_Data_3));
            err = send(sock, tx_data, sizeof(Sensor_Data_3), 0);
            break;
        case 4:
            data4 = get_protocol_4((int8_t) status);
            memcpy(tx_data, &data4, sizeof(Sensor_Data_4));
            err = send(sock, tx_data, sizeof(Sensor_Data_4), 0);
            break;
        case 5:
            data5 = get_protocol_5((int8_t) status);
            memcpy(tx_data, &data5, sizeof(Sensor_Data_5));
            err = send(sock, tx_data, sizeof(Sensor_Data_5), 0);
            break;
        default:
            break;
        }

        // RECEIVE DATA
        int len = recv(sock, readBuffer, sizeof(readBuffer), 0);
        new_status = (int32_t) readBuffer[0];
        new_protocol = (int32_t) readBuffer[1];

        // CHANGE PROTOCOL
        if (protocol != new_protocol) {
            Write_NVS(new_protocol, 2);
        }

        // CHANGE STATUS
        if (status != new_status) {
            Write_NVS(new_status, 1);
            return TCP_SUCCESS;
        }

        // DEEP SLEEP
        if (status == 22) {
            return TCP_SUCCESS;
        }
    }

    return TCP_SUCCESS;
}
