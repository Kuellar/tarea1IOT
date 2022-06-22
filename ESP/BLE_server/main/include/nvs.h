#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"

int Write_NVS(int32_t data, int key);
esp_err_t Write_NVS_str(char* data, int key);
esp_err_t Read_NVS(int32_t* data,int key);
esp_err_t Read_NVS_str(char* data, int key, size_t *length);