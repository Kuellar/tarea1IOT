#include "BLE_server.h"

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
    BLE_server_Init(500); //It was the default value in the example

    tx_data = (uint8_t*) malloc(sizeof(uint8_t));
    tx_data[0] = 0;
    tx_len = 1;

    uint8_t sendsCounter = 0;
    
    static bool inicio = true;
    while(1){
        vTaskDelay(200);
        if(is_Aconnected && inicio){
            printf("Connection established\n");
            printf("gatts_if %d, conn_id: %d, handle %d\n", gl_profile_tab[PROFILE_A_APP_ID].gatts_if, gl_profile_tab[PROFILE_A_APP_ID].conn_id, gl_profile_tab[PROFILE_A_APP_ID].char_handle); 
            inicio = false;
        }

        if(is_Aconnected && notificationA_enable){
            sendsCounter++;
            tx_len = 3;
            tx_data = (uint8_t*) malloc(sizeof(uint8_t) * 3);
            tx_data[0] = 83;
            tx_data[1] = 1;
            tx_data[2] = sendsCounter;
            ESP_ERROR_CHECK(BLE_send(tx_len, tx_data, false));
            

            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
    return;
}

