#include "data.h"
#include "esp_system.h"
#include <stdint.h>
#include <string.h>
#include <math.h>

Sensor_Data_0 get_protocol_0(int8_t status) {
    // READ BASE MAC
    uint8_t base_mac[6];
    esp_base_mac_addr_get(base_mac);

    Sensor_Data_0 res;
    // HEADER
    res.ID_protocolo = 0;
    res.Status = status;
    memcpy(res.MAC, base_mac, sizeof(res.MAC));
    res.Leng_msg = 1;
    // DATA
    res.Ok = 1;

    return res;
}


Sensor_Data_1 get_protocol_1(int8_t status) {
    // READ BASE MAC
    uint8_t base_mac[6];
    esp_base_mac_addr_get(base_mac);
    // esp_random() -> uint32_t

    Sensor_Data_1 res;
    // HEADER
    res.ID_protocolo = 1;
    res.Status = status;
    memcpy(res.MAC, base_mac, sizeof(res.MAC));
    res.Leng_msg = 6;
    // DATA
    res.Batt_level = abs((int8_t) esp_random()) % 100;
    res.Timestamp[0] = res.Timestamp[1] = res.Timestamp[2] = res.Timestamp[3] = 0;

    return res;
}

Sensor_Data_2 get_protocol_2(int8_t status) {
    // READ BASE MAC
    uint8_t base_mac[6];
    esp_base_mac_addr_get(base_mac);

    Sensor_Data_2 res;
    // HEADER
    res.ID_protocolo = 2;
    res.Status = status;
    memcpy(res.MAC, base_mac, sizeof(res.MAC));
    res.Leng_msg = 16;
    // DATA
    res.Batt_level = abs((int8_t) esp_random()) % 100;
    res.Timestamp[0] = res.Timestamp[1] = res.Timestamp[2] = res.Timestamp[3] = 0;

    res.Temp = (int8_t) esp_random() % 25 + 5;
    int32_t press = (int32_t) (esp_random() / UINT32_MAX) * 200 + 1000;
    memcpy(res.Press, &press, sizeof(res.Press));
    res.Hum = (int8_t) esp_random() % 50 + 30;
    int32_t co = (int32_t) (esp_random() / UINT32_MAX) * 170 + 30;
    memcpy(res.Co, &co, sizeof(res.Co));

    return res;
}

Sensor_Data_3 get_protocol_3(int8_t status) {
    // READ BASE MAC
    uint8_t base_mac[6];
    esp_base_mac_addr_get(base_mac);

    // SENSORS DATA
    int32_t Amp_x = ((int32_t)esp_random() / UINT32_MAX) * (0.12 - 0.0059) + 0.0059;
    int32_t Amp_y = ((int32_t)esp_random() / UINT32_MAX) * (0.11 - 0.0041) + 0.0041;
    int32_t Amp_z = ((int32_t)esp_random() / UINT32_MAX) * (0.15 - 0.008) + 0.008;
    int32_t RMS = sqrtf(powf(Amp_x, 2) + powf(Amp_y, 2) + powf(Amp_z, 2));

    Sensor_Data_3 res;
    // HEADER
    res.ID_protocolo = 3;
    res.Status = status;
    memcpy(res.MAC, base_mac, sizeof(res.MAC));
    res.Leng_msg = 20;
    // DATA
    res.Batt_level = abs((int8_t) esp_random()) % 100;
    res.Timestamp[0] = res.Timestamp[1] = res.Timestamp[2] = res.Timestamp[3] = 0;

    res.Temp = (int8_t) esp_random() % 25 + 5;
    int32_t press = (int32_t) (esp_random() / UINT32_MAX) * 200 + 1000;
    memcpy(res.Press, &press, sizeof(res.Press));
    res.Hum = (int8_t) esp_random() % 50 + 30;
    int32_t co = (int32_t) (esp_random() / UINT32_MAX) * 170 + 30;
    memcpy(res.Co, &co, sizeof(res.Co));

    memcpy(res.RMS, &RMS, sizeof(res.RMS));

    return res;
}

Sensor_Data_4 get_protocol_4(int8_t status) {
    // READ BASE MAC
    uint8_t base_mac[6];
    esp_base_mac_addr_get(base_mac);

    // SENSORS DATA
    int32_t Amp_x = ((int32_t)esp_random() / UINT32_MAX) * (0.12 - 0.0059) + 0.0059;
    int32_t Frec_x = ((int32_t)esp_random() / UINT32_MAX) * (31 - 29) + 29;
    int32_t Amp_y = ((int32_t)esp_random() / UINT32_MAX) * (0.11 - 0.0041) + 0.0041;
    int32_t Frec_y = ((int32_t)esp_random() / UINT32_MAX) * (61 - 59) + 59;
    int32_t Amp_z = ((int32_t)esp_random() / UINT32_MAX) * (0.15 - 0.008) + 0.008;
    int32_t Frec_z = ((int32_t)esp_random() / UINT32_MAX) * (91 - 89) + 89;
    int32_t RMS = sqrtf(powf(Amp_x, 2) + powf(Amp_y, 2) + powf(Amp_z, 2));

    Sensor_Data_4 res;
    // HEADER
    res.ID_protocolo = 4;
    res.Status = status;
    memcpy(res.MAC, base_mac, sizeof(res.MAC));
    res.Leng_msg = 44;
    // DATA
    res.Batt_level = abs((int8_t) esp_random()) % 100;
    res.Timestamp[0] = res.Timestamp[1] = res.Timestamp[2] = res.Timestamp[3] = 0;

    res.Temp = (int8_t) esp_random() % 25 + 5;
    int32_t press = (int32_t) (esp_random() / UINT32_MAX) * 200 + 1000;
    memcpy(res.Press, &press, sizeof(res.Press));
    res.Hum = (int8_t) esp_random() % 50 + 30;
    int32_t co = (int32_t) (esp_random() / UINT32_MAX) * 170 + 30;
    memcpy(res.Co, &co, sizeof(res.Co));

    memcpy(res.RMS, &RMS, sizeof(res.RMS));
    memcpy(res.Amp_x_bmi, &Amp_x, sizeof(res.Amp_x_bmi));
    memcpy(res.Frec_x_bmi, &Frec_x, sizeof(res.Frec_x_bmi));
    memcpy(res.Amp_y_bmi, &Amp_y, sizeof(res.Amp_y_bmi));
    memcpy(res.Frec_y_bmi, &Frec_y, sizeof(res.Frec_y_bmi));
    memcpy(res.Amp_z_bmi, &Amp_z, sizeof(res.Amp_z_bmi));
    memcpy(res.Frec_z_bmi, &Frec_z, sizeof(res.Frec_z_bmi));

    return res;
}

Sensor_Data_5 get_protocol_5(int8_t status) {
    // READ BASE MAC
    uint8_t base_mac[6];
    esp_base_mac_addr_get(base_mac);

    Sensor_Data_5 res;
    // HEADER
    res.ID_protocolo = 5;
    res.Status = status;
    memcpy(res.MAC, base_mac, sizeof(res.MAC));
    res.Leng_msg = 0; // Not enough memory space for 12016
    // DATA
    res.Batt_level = abs((int8_t) esp_random()) % 100;
    res.Timestamp[0] = res.Timestamp[1] = res.Timestamp[2] = res.Timestamp[3] = 0;

    res.Temp = (int8_t) esp_random() % 25 + 5;
    int32_t press = (int32_t) (esp_random() / UINT32_MAX) * 200 + 1000;
    memcpy(res.Press, &press, sizeof(res.Press));
    res.Hum = (int8_t) esp_random() % 50 + 30;
    int32_t co = (int32_t) (esp_random() / UINT32_MAX) * 170 + 30;
    memcpy(res.Co, &co, sizeof(res.Co));

    for (int i = 0; i < 2000; i++)
    {
        res.BMI270_Acc_x[i] = (int16_t) (esp_random() / UINT32_MAX) * 1600 - 8000;
        res.BMI270_Acc_y[i] = (int16_t) (esp_random() / UINT32_MAX) * 1600 - 8000;
        res.BMI270_Acc_z[i] = (int16_t) (esp_random() / UINT32_MAX) * 1600 - 8000;
    }

    return res;
}