#include "BLE_server.h"
#include "nvs.h"
#include "data.h"

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
        "MAC wifi: %d:%d:%d:%d:%d:%d\n",
        mac_wifi[0],mac_wifi[1],mac_wifi[2],mac_wifi[3],mac_wifi[4],mac_wifi[5]
    );
    printf(
        "MAC bt: %d:%d:%d:%d:%d:%d\n",
        mac_bt[0],mac_bt[1],mac_bt[2],mac_bt[3],mac_bt[4],mac_bt[5]
    );

    // READ STATUS
    int32_t status, new_status;
    Read_NVS(&status, 1);

    // VARS
    Sensor_Data_1 data1;
    tx_data = (uint8_t*) malloc(sizeof(Sensor_Data_1));

    switch (status)
    {
    case 20:
        /* Configuración vía TCP en BD  */
        break;
    case 21:
        /* Conexión TCP continua  */
        break;
    case 22:
        /* Conexión TCP discontinua */
        break;
    case 23:
        /* Conexión UDP */
        break;
    case 30:
        /* BLE continua */
        BLE_server_Init(500); //It was the default value in the example

        bool inicio_status_30 = true;
        while(1){
            vTaskDelay(200);
            if(is_Aconnected && inicio_status_30){
                printf("Connection established\n");
                printf("gatts_if %d, conn_id: %d, handle %d\n", gl_profile_tab[PROFILE_A_APP_ID].gatts_if, gl_profile_tab[PROFILE_A_APP_ID].conn_id, gl_profile_tab[PROFILE_A_APP_ID].char_handle); 
                inicio_status_30 = false;
                printf("Sending data protocol 1...\n");
            }

            int32_t protocol;
            Read_NVS(&protocol, 2);

            if(is_Aconnected && notificationA_enable){
                switch (protocol)
                {
                case 1:
                    tx_len = 14;
                    data1 = get_protocol_1((int8_t) status);
                    memcpy(tx_data, &data1, sizeof(Sensor_Data_1));

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

