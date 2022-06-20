#include "data.h"
#include "esp_system.h"
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>

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

    res.Temp = abs((int8_t) esp_random()) % 25 + 5;
    int32_t press = abs((int32_t) esp_random()) % 200 + 1000;
    res.Press[3] = (uint8_t)press;
    res.Press[2] = (uint8_t)(press>>=8);
    res.Press[1] = (uint8_t)(press>>=8);
    res.Press[0] = (uint8_t)(press>>=8);
    res.Hum = abs((int8_t) esp_random()) % 50 + 30;
    int32_t co = abs((int32_t) esp_random()) % 170 + 30;
    res.Co[3] = (uint8_t)co;
    res.Co[2] = (uint8_t)(co>>=8);
    res.Co[1] = (uint8_t)(co>>=8);
    res.Co[0] = (uint8_t)(co>>=8);

    return res;
}


Sensor_Data_3 get_protocol_3(int8_t status) {
    // READ BASE MAC
    uint8_t base_mac[6];
    esp_base_mac_addr_get(base_mac);

    // SENSORS DATA
    float Amp_xf = ((float)esp_random() / UINT32_MAX) * (0.12 - 0.0059) + 0.0059;
    float Amp_yf = ((float)esp_random() / UINT32_MAX) * (0.11 - 0.0041) + 0.0041;
    float Amp_zf = ((float)esp_random() / UINT32_MAX) * (0.15 - 0.008) + 0.008;

    Sensor_Data_3 res;
    // HEADER
    res.ID_protocolo = 3;
    res.Status = status;
    memcpy(res.MAC, base_mac, sizeof(res.MAC));
    res.Leng_msg = 20;
    // DATA
    res.Batt_level = abs((int8_t) esp_random()) % 100;
    res.Timestamp[0] = res.Timestamp[1] = res.Timestamp[2] = res.Timestamp[3] = 0;

    res.Temp = abs((int8_t) esp_random()) % 25 + 5;
    int32_t press = abs((int32_t) esp_random()) % 200 + 1000;
    res.Press[3] = (uint8_t)press;
    res.Press[2] = (uint8_t)(press>>=8);
    res.Press[1] = (uint8_t)(press>>=8);
    res.Press[0] = (uint8_t)(press>>=8);
    res.Hum = abs((int8_t) esp_random()) % 50 + 30;
    int32_t co = abs((int32_t) esp_random()) % 170 + 30;
    res.Co[3] = (uint8_t)co;
    res.Co[2] = (uint8_t)(co>>=8);
    res.Co[1] = (uint8_t)(co>>=8);
    res.Co[0] = (uint8_t)(co>>=8);

    int32_t RMS = sqrtf(powf(Amp_xf, 2) + powf(Amp_yf, 2) + powf(Amp_zf, 2)) * 100000000;
    res.RMS[3] = (uint8_t)RMS;
    res.RMS[2] = (uint8_t)(RMS>>=8);
    res.RMS[1] = (uint8_t)(RMS>>=8);
    res.RMS[0] = (uint8_t)(RMS>>=8);

    return res;
}


Sensor_Data_4 get_protocol_4(int8_t status) {
    // READ BASE MAC
    uint8_t base_mac[6];
    esp_base_mac_addr_get(base_mac);

    // SENSORS DATA
    float Amp_xf = ((float)esp_random() / UINT32_MAX) * (0.12 - 0.0059) + 0.0059;
    float Frec_xf = ((float)esp_random() / UINT32_MAX) * (31 - 29) + 29;
    float Amp_yf = ((float)esp_random() / UINT32_MAX) * (0.11 - 0.0041) + 0.0041;
    float Frec_yf = ((float)esp_random() / UINT32_MAX) * (61 - 59) + 59;
    float Amp_zf = ((float)esp_random() / UINT32_MAX) * (0.15 - 0.008) + 0.008;
    float Frec_zf = ((float)esp_random() / UINT32_MAX) * (91 - 89) + 89;

    Sensor_Data_4 res;
    // HEADER
    res.ID_protocolo = 4;
    res.Status = status;
    memcpy(res.MAC, base_mac, sizeof(res.MAC));
    res.Leng_msg = 44;
    // DATA
    res.Batt_level = abs((int8_t) esp_random()) % 100;
    res.Timestamp[0] = res.Timestamp[1] = res.Timestamp[2] = res.Timestamp[3] = 0;

    res.Temp = abs((int8_t) esp_random()) % 25 + 5;
    int32_t press = abs((int32_t) esp_random()) % 200 + 1000;
    res.Press[3] = (uint8_t)press;
    res.Press[2] = (uint8_t)(press>>=8);
    res.Press[1] = (uint8_t)(press>>=8);
    res.Press[0] = (uint8_t)(press>>=8);
    res.Hum = abs((int8_t) esp_random()) % 50 + 30;
    int32_t co = abs((int32_t) esp_random()) % 170 + 30;
    res.Co[3] = (uint8_t)co;
    res.Co[2] = (uint8_t)(co>>=8);
    res.Co[1] = (uint8_t)(co>>=8);
    res.Co[0] = (uint8_t)(co>>=8);

    int32_t RMS = sqrtf(powf(Amp_xf, 2) + powf(Amp_yf, 2) + powf(Amp_zf, 2)) * 100000000;
    res.RMS[3] = (uint8_t)RMS;
    res.RMS[2] = (uint8_t)(RMS>>=8);
    res.RMS[1] = (uint8_t)(RMS>>=8);
    res.RMS[0] = (uint8_t)(RMS>>=8);

    int32_t Amp_x = Amp_xf * 100000000;
    res.Amp_x_bmi[3] = (uint8_t)Amp_x;
    res.Amp_x_bmi[2] = (uint8_t)(Amp_x>>=8);
    res.Amp_x_bmi[1] = (uint8_t)(Amp_x>>=8);
    res.Amp_x_bmi[0] = (uint8_t)(Amp_x>>=8);

    int32_t Frec_x = Frec_xf * 1000000;
    res.Frec_x_bmi[3] = (uint8_t)Frec_x;
    res.Frec_x_bmi[2] = (uint8_t)(Frec_x>>=8);
    res.Frec_x_bmi[1] = (uint8_t)(Frec_x>>=8);
    res.Frec_x_bmi[0] = (uint8_t)(Frec_x>>=8);

    int32_t Amp_y = Amp_yf * 100000000;
    res.Amp_y_bmi[3] = (uint8_t)Amp_y;
    res.Amp_y_bmi[2] = (uint8_t)(Amp_y>>=8);
    res.Amp_y_bmi[1] = (uint8_t)(Amp_y>>=8);
    res.Amp_y_bmi[0] = (uint8_t)(Amp_y>>=8);

    int32_t Frec_y = Frec_yf * 1000000;
    res.Frec_y_bmi[3] = (uint8_t)Frec_y;
    res.Frec_y_bmi[2] = (uint8_t)(Frec_y>>=8);
    res.Frec_y_bmi[1] = (uint8_t)(Frec_y>>=8);
    res.Frec_y_bmi[0] = (uint8_t)(Frec_y>>=8);

    int32_t Amp_z = Amp_zf * 100000000;
    res.Amp_z_bmi[3] = (uint8_t)Amp_z;
    res.Amp_z_bmi[2] = (uint8_t)(Amp_z>>=8);
    res.Amp_z_bmi[1] = (uint8_t)(Amp_z>>=8);
    res.Amp_z_bmi[0] = (uint8_t)(Amp_z>>=8);

    int32_t Frec_z = Frec_zf * 1000000;
    res.Frec_z_bmi[3] = (uint8_t)Frec_z;
    res.Frec_z_bmi[2] = (uint8_t)(Frec_z>>=8);
    res.Frec_z_bmi[1] = (uint8_t)(Frec_z>>=8);
    res.Frec_z_bmi[0] = (uint8_t)(Frec_z>>=8);

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

    res.Temp = abs((int8_t) esp_random()) % 25 + 5;
    int32_t press = abs((int32_t) esp_random()) % 200 + 1000;
    res.Press[3] = (uint8_t)press;
    res.Press[2] = (uint8_t)(press>>=8);
    res.Press[1] = (uint8_t)(press>>=8);
    res.Press[0] = (uint8_t)(press>>=8);
    res.Hum = abs((int8_t) esp_random()) % 50 + 30;
    int32_t co = abs((int32_t) esp_random()) % 170 + 30;
    res.Co[3] = (uint8_t)co;
    res.Co[2] = (uint8_t)(co>>=8);
    res.Co[1] = (uint8_t)(co>>=8);
    res.Co[0] = (uint8_t)(co>>=8);

    for (int i = 0; i < 2000; i++)
    {
        res.BMI270_Acc_x[i] = (int16_t) (esp_random() / UINT32_MAX) * 1600 - 8000;
        res.BMI270_Acc_y[i] = (int16_t) (esp_random() / UINT32_MAX) * 1600 - 8000;
        res.BMI270_Acc_z[i] = (int16_t) (esp_random() / UINT32_MAX) * 1600 - 8000;
    }

    return res;
}
