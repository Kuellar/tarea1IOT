#include <stdint.h>

typedef struct Config
{
    int8_t status;
    int8_t ID_Protocol;
    int32_t BMI270_Sampling;
    int32_t BMI270_Acc_Sensibility;
    int32_t BMI270_Gyro_Sensibility;
    int32_t BME688_Sampling;
    int32_t Discontinuous_Time;
    int32_t Port_TCP;
    int32_t Port_UDP;
    int32_t Host_Ip_Addr;
    int32_t Ssid;
    int32_t Pass;
}Config;

#define BYTE sizeof(int8_t)
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
#define DATA_7_SPACE 4
#define DATA_8_SPACE 4
#define DATA_9_SPACE 4
#define DATA_10_SPACE 4
#define DATA_11_SPACE 4
#define DATA_12_SPACE 4
#define DATA_13_SPACE 4

typedef struct Sensor_Data_0
{
    // HEADER
    int8_t ID_protocolo;
    int8_t Status;
    int8_t MAC[6];
    int8_t Leng_msg;
    // DATA
    int8_t Ok;
} Sensor_Data_0;

typedef struct Sensor_Data_1
{
    // HEADER
    int8_t ID_protocolo;
    int8_t Status;
    int8_t MAC[6];
    int8_t Leng_msg;
    // DATA
    int8_t Batt_level;
    int8_t Timestamp[4];
} Sensor_Data_1;

typedef struct Sensor_Data_2
{
    // HEADER
    int8_t ID_protocolo;
    int8_t Status;
    int8_t MAC[6];
    int8_t Leng_msg;
    // DATA
    int8_t Batt_level;
    int8_t Timestamp[4];
    int8_t Temp;
    int8_t Press[4];
    int8_t Hum;
    int8_t Co[4];
} Sensor_Data_2;

typedef struct Sensor_Data_3
{
    // HEADER
    int8_t ID_protocolo;
    int8_t Status;
    int8_t MAC[6];
    int8_t Leng_msg;
    // DATA
    int8_t Batt_level;
    int8_t Timestamp[4];
    int8_t Temp;
    int8_t Press[4];
    int8_t Hum;
    int8_t Co[4];
    int8_t RMS[4];
} Sensor_Data_3;

typedef struct Sensor_Data_4
{
    // HEADER
    int8_t ID_protocolo;
    int8_t Status;
    int8_t MAC[6];
    int8_t Leng_msg;
    // DATA
    int8_t Batt_level;
    int8_t Timestamp[4];
    int8_t Temp;
    int8_t Press[4];
    int8_t Hum;
    int8_t Co[4];
    int8_t RMS[4];
    int8_t Amp_x_bmi[4];
    int8_t Frec_x_bmi[4];
    int8_t Amp_y_bmi[4];
    int8_t Frec_y_bmi[4];
    int8_t Amp_z_bmi[4];
    int8_t Frec_z_bmi[4];
} Sensor_Data_4;

typedef struct Sensor_Data_5
{
    // HEADER
    int8_t ID_protocolo;
    int8_t Status;
    int8_t MAC[6];
    int8_t Leng_msg;
    // DATA
    int8_t Batt_level;
    int8_t Timestamp[4];
    int8_t Temp;
    int8_t Press[4];
    int8_t Hum;
    int8_t Co[4];
    int16_t BMI270_Acc_x[2000];
    int16_t BMI270_Acc_y[2000];
    int16_t BMI270_Acc_z[2000];
    // int16_t BMI270_Gir_x[2000];
    // int16_t BMI270_Gir_y[2000];
    // int16_t BMI270_Gir_z[2000];
} Sensor_Data_5;

Sensor_Data_0 get_protocol_0(int8_t status);
Sensor_Data_1 get_protocol_1(int8_t status);
Sensor_Data_2 get_protocol_2(int8_t status);
Sensor_Data_3 get_protocol_3(int8_t status);
Sensor_Data_4 get_protocol_4(int8_t status);
Sensor_Data_5 get_protocol_5(int8_t status);
