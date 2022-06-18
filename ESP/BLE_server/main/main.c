#include "BLE_server.h"
#include "nvs.h"

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
    unsigned long long mac_wifi;
    unsigned long long mac_bt;
    esp_read_mac(&mac_wifi, ESP_MAC_BT);
    esp_read_mac(&mac_bt, ESP_MAC_WIFI_STA);
    printf("MAC wifi: %lld\n", mac_wifi);
    printf("MAC bt: %lld\n", mac_bt);

    // READ STATUS
    int32_t status, new_status;
    Read_NVS(&status, 1);

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

        tx_data = (uint8_t*) malloc(sizeof(uint8_t));
        tx_data[0] = 0;
        tx_len = 1;
        uint8_t sendsCounter = 0;

        bool inicio_status_30 = true;
        while(1){
            vTaskDelay(200);
            if(is_Aconnected && inicio_status_30){
                printf("Connection established\n");
                printf("gatts_if %d, conn_id: %d, handle %d\n", gl_profile_tab[PROFILE_A_APP_ID].gatts_if, gl_profile_tab[PROFILE_A_APP_ID].conn_id, gl_profile_tab[PROFILE_A_APP_ID].char_handle); 
                inicio_status_30 = false;
            }

            int32_t data;
            Read_NVS(&data, 1);

            if(is_Aconnected && notificationA_enable){
                sendsCounter++;
                tx_len = 3;
                tx_data = (uint8_t*) malloc(sizeof(uint8_t) * 3);
                tx_data[0] = 83;
                tx_data[1] = 1;
                tx_data[2] = sendsCounter;
                printf("Sending data...\n");
                printf(" data... %d\n", data);
                ESP_ERROR_CHECK(BLE_send(tx_len, tx_data, false));

                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }

            Read_NVS(&new_status, 1);
            if (status != new_status) break;
        }
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

