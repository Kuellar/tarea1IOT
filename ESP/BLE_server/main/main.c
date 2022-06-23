#include "BLE_server.h"
#include "WIFI_client.h"
#include "nvs.h"
#include "data.h"
#include <inttypes.h>
#include "esp_sleep.h"

//Varibles to know the BLE status and data exchange
extern uint8_t *rx_data, rx_len;
extern bool write_EVT;

extern uint8_t *tx_data, tx_len;
extern bool read_EVT;

extern bool is_Aconnected;
extern bool notificationA_enable;

extern struct gatts_profile_inst gl_profile_tab[PROFILE_NUM];

void app_main(void)
{
    // GET MAC
    uint8_t mac_wifi[6];
    uint8_t mac_bt[6];
    esp_read_mac(mac_wifi, ESP_MAC_WIFI_STA);
    esp_read_mac(mac_bt, ESP_MAC_BT);
    printf(
        "MAC wifi: %X:%X:%X:%X:%X:%X\n",
        mac_wifi[0],mac_wifi[1],mac_wifi[2],mac_wifi[3],mac_wifi[4],mac_wifi[5]
    );
    printf(
        "MAC bt: %X:%X:%X:%X:%X:%X\n",
        mac_bt[0],mac_bt[1],mac_bt[2],mac_bt[3],mac_bt[4],mac_bt[5]
    );
    // WIFI
    esp_err_t wifi_status;

    // READ STATUS
    int32_t status, new_status, protocol, deep_sleep_time;
    esp_err_t err_read_status;

    err_read_status = Read_NVS(&status, 1);
    if (err_read_status == ESP_ERR_NVS_NOT_FOUND) {
        Write_NVS(0, 1);  // STATUS 0
        Write_NVS(0, 2);  // PROTOCOL 0
        Read_NVS(&status, 1);
    }

    // VARS
    // Sensor_Data_0 data0;
    Sensor_Data_1 data1;
    Sensor_Data_2 data2;
    Sensor_Data_3 data3;
    Sensor_Data_4 data4;
    // Sensor_Data_5 data5;
    tx_data = (uint8_t*) malloc(sizeof(Sensor_Data_4));

    switch (status)
    {
    case 20:
        /* Configuración vía TCP en BD  */
        wifi_status = connect_wifi();
        if (WIFI_SUCCESS != wifi_status) {
            Write_NVS(0, 1);  // STATUS 0
            Write_NVS(0, 2);  // PROTOCOL 0
            break;
        }

        for (int i = 0; i < 3; i++){
            wifi_status = connect_TCP_server(status);
            if (TCP_SUCCESS == wifi_status) break;
        }
        if (TCP_SUCCESS != wifi_status) {
            ESP_LOGI(WIFI_TAG, "Failed to connect to remote server");
            Write_NVS(0, 1);  // STATUS 0
            Write_NVS(0, 2);  // PROTOCOL 0
        }

        vTaskDelay(1000);
        break;
    case 21:
        /* Conexión TCP continua  */
        wifi_status = connect_wifi();
        if (WIFI_SUCCESS != wifi_status) {
            Write_NVS(0, 1);  // STATUS 0
            Write_NVS(0, 2);  // PROTOCOL 0
            break;
        }

        for (int i = 0; i < 3; i++){
            wifi_status = connect_TCP_server(status);
            if (TCP_SUCCESS == wifi_status) break;
        }
        if (TCP_SUCCESS != wifi_status) {
            ESP_LOGI(WIFI_TAG, "Failed to connect to remote server");
            Write_NVS(0, 1);  // STATUS 0
            Write_NVS(0, 2);  // PROTOCOL 0
        }

        vTaskDelay(1000);
        break;
    case 22:
        /* Conexión TCP discontinua */
        wifi_status = connect_wifi();
        if (WIFI_SUCCESS != wifi_status) {
            Write_NVS(0, 1);  // STATUS 0
            Write_NVS(0, 2);  // PROTOCOL 0
            break;
        }

        for (int i = 0; i < 3; i++){
            wifi_status = connect_TCP_server(status);
            if (TCP_SUCCESS == wifi_status) break;
        }
        if (TCP_SUCCESS != wifi_status) {
            ESP_LOGI(WIFI_TAG, "Failed to connect to remote server");
            Write_NVS(0, 1);  // STATUS 0
            Write_NVS(0, 2);  // PROTOCOL 0
        }

        err_read_status = Read_NVS(&deep_sleep_time, 7);
        if (err_read_status == ESP_ERR_NVS_NOT_FOUND || deep_sleep_time > 60) {
            printf("ERROR: Deep Sleep\n");
            Write_NVS(0, 1);  // STATUS 0
            Write_NVS(0, 2);  // PROTOCOL 0
        }
        printf("Deep sleep for: %" PRIu32 " seconds\n", deep_sleep_time);
        ESP_ERROR_CHECK(esp_wifi_stop());
        esp_deep_sleep(1e6 * deep_sleep_time); // microsecond
        break;
    case 23:
        /* Conexión UDP */
        wifi_status = connect_wifi();
        if (WIFI_SUCCESS != wifi_status) {
            Write_NVS(0, 1);  // STATUS 0
            Write_NVS(0, 2);  // PROTOCOL 0
            break;
        }

        for (int i = 0; i < 3; i++){
            wifi_status = connect_UDP_server(status);
            if (UDP_SUCCESS == wifi_status) break;
        }
        if (UDP_SUCCESS != wifi_status) {
            ESP_LOGI(WIFI_TAG, "Failed to connect to remote server");
            Write_NVS(0, 1);  // STATUS 0
            Write_NVS(0, 2);  // PROTOCOL 0
        }


        vTaskDelay(1000);
        break;
    case 30:
        /* BLE continua */
        BLE_server_Init(500); //It was the default value in the example

        bool inicio_status_30 = true;
        while(1){
            vTaskDelay(200);
            Read_NVS(&protocol, 2);

            if(is_Aconnected && inicio_status_30){
                printf("Connection established\n");
                printf("gatts_if %d, conn_id: %d, handle %d\n", gl_profile_tab[PROFILE_A_APP_ID].gatts_if, gl_profile_tab[PROFILE_A_APP_ID].conn_id, gl_profile_tab[PROFILE_A_APP_ID].char_handle); 
                inicio_status_30 = false;
                printf("Sending data protocol: %" PRIu32 "\n",protocol);
            }

            if(is_Aconnected && notificationA_enable){
                switch (protocol)
                {
                case 1:
                    tx_len = 14;
                    data1 = get_protocol_1((int8_t) status);
                    memcpy(tx_data, &data1, sizeof(Sensor_Data_1));
                    ESP_ERROR_CHECK(BLE_send(tx_len, tx_data, false));
                    break;
                case 2:
                    tx_len = 24;
                    data2 = get_protocol_2((int8_t) status);
                    memcpy(tx_data, &data2, sizeof(Sensor_Data_2));
                    ESP_ERROR_CHECK(BLE_send(tx_len, tx_data, false));
                    break;
                case 3:
                    tx_len = 28;
                    data3 = get_protocol_3((int8_t) status);
                    memcpy(tx_data, &data3, sizeof(Sensor_Data_3));
                    ESP_ERROR_CHECK(BLE_send(tx_len, tx_data, false));
                    break;
                case 4:
                    tx_len = 52;
                    data4 = get_protocol_4((int8_t) status);
                    memcpy(tx_data, &data4, sizeof(Sensor_Data_4));
                    ESP_ERROR_CHECK(BLE_send(tx_len, tx_data, false));
                    break;
                default:
                    break;
                }
                printf(".");
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }

            Read_NVS(&new_status, 1);
            if (status != new_status) break;
        }
        printf("\n");
        break;
    case 31:
        /* BLE discontinua */
        BLE_server_Init(500);

        vTaskDelay(200);
        Read_NVS(&protocol, 2);

        // WAIT FOR CONECTION
        while(!is_Aconnected) {
            vTaskDelay(500);
        }

        printf("Connection established\n");
        printf("gatts_if %d, conn_id: %d, handle %d\n", gl_profile_tab[PROFILE_A_APP_ID].gatts_if, gl_profile_tab[PROFILE_A_APP_ID].conn_id, gl_profile_tab[PROFILE_A_APP_ID].char_handle);
        printf("Sending data protocol: %" PRIu32 "\n", protocol);
        vTaskDelay(500);

        if(notificationA_enable){
            switch (protocol)
            {
            case 1:
                tx_len = 14;
                data1 = get_protocol_1((int8_t) status);
                memcpy(tx_data, &data1, sizeof(Sensor_Data_1));
                ESP_ERROR_CHECK(BLE_send(tx_len, tx_data, false));
                break;
            case 2:
                tx_len = 24;
                data2 = get_protocol_2((int8_t) status);
                memcpy(tx_data, &data2, sizeof(Sensor_Data_2));
                ESP_ERROR_CHECK(BLE_send(tx_len, tx_data, false));
                break;
            case 3:
                tx_len = 28;
                data3 = get_protocol_3((int8_t) status);
                memcpy(tx_data, &data3, sizeof(Sensor_Data_3));
                ESP_ERROR_CHECK(BLE_send(tx_len, tx_data, false));
                break;
            case 4:
                tx_len = 52;
                data4 = get_protocol_4((int8_t) status);
                memcpy(tx_data, &data4, sizeof(Sensor_Data_4));
                ESP_ERROR_CHECK(BLE_send(tx_len, tx_data, false));

                break;
            default:
                break;
            }
        }
        vTaskDelay(500);

        Read_NVS(&new_status, 1);
        if (status != new_status) break;

        err_read_status = Read_NVS(&deep_sleep_time, 7);
        if (err_read_status == ESP_ERR_NVS_NOT_FOUND || deep_sleep_time > 60) {
            printf("ERROR: Deep Sleep\n");
            Write_NVS(0, 1);  // STATUS 0
            Write_NVS(0, 2);  // PROTOCOL 0
        }
        printf("Deep sleep for: %" PRIu32 " seconds\n", deep_sleep_time);
        ESP_ERROR_CHECK(esp_bt_controller_disable());
        esp_deep_sleep(1e6 * deep_sleep_time); // microsecond
        break;
    default:
        /* Configuración por Bluetooth */
        BLE_server_Init(500); //It was the default value in the example
        bool inicio_status_0 = true;
        while(1){
            vTaskDelay(200);
            if(is_Aconnected && inicio_status_0){
                printf("Connection established\n");
                printf(
                    "gatts_if %d, conn_id: %d, handle %d\n",
                    gl_profile_tab[PROFILE_A_APP_ID].gatts_if,
                    gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                    gl_profile_tab[PROFILE_A_APP_ID].char_handle
                );
                inicio_status_0 = false;
            }
            Read_NVS(&new_status, 1);
            if (status != new_status) break;
        }
        break;
    }

    printf("END\n");
    esp_restart();

   return;
}

