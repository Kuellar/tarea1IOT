SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "-04:00";

/* SIZE
tinyint   1 byte - int8_t
smallint  2 byte - int16_t
mediumint 3 byte - int24_t
int       4 byte - int32_t
bigint    8 byte - int64_t

timestamp 4 byte
*/

CREATE DATABASE `tarea_iot`;
USE tarea_iot;

/* Configuración por Bluetooth (status=0) */
CREATE TABLE `config` (
    `Status` tinyint NOT NULL,
    `ID_Protocol` tinyint NOT NULL,
    `BMI270_sampling` int NOT NULL,
    `BMI270_Acc_Sensibility` int NOT NULL,
    `BMI270_Gyro_Sensibility` int NOT NULL,
    `BME688_Sampling` int NOT NULL,
    `Discontinuous_Time` int NOT NULL,
    `Port_TCP` int NOT NULL,
    `Port_UDP` int NOT NULL,
    `Host_Ip_Addr` int NOT NULL,
    `Ssid` int NOT NULL, /* KEY? */
    `Pass` int NOT NULL,

    /* KEY */
    PRIMARY KEY(Ssid)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE `protocol_0` (
    `id` int NOT NULL AUTO_INCREMENT,
    `esp_ssid` int, /* CHECK */
    /* DATA */
    `ok` tinyint NOT NULL,

    /* KEY */
    PRIMARY KEY(id),
    FOREIGN KEY(esp_ssid) REFERENCES config(Ssid)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE `protocol_1` (
    `id` int NOT NULL AUTO_INCREMENT,
    `esp_ssid` int NOT NULL, /*CHECK*/
    /* DATA */
    `Batt_level` tinyint NOT NULL,
    `Timestamp` timestamp DEFAULT CURRENT_TIMESTAMP,

    /* KEY */
    PRIMARY KEY(id),
    FOREIGN KEY(esp_ssid) REFERENCES config(Ssid)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE `protocol_2` (
    `id` int NOT NULL AUTO_INCREMENT,
    `esp_ssid` int NOT NULL, /*CHECK*/
    /* DATA */
    `Batt_level` tinyint NOT NULL,
    `Timestamp` timestamp DEFAULT CURRENT_TIMESTAMP,
    `Temp` tinyint NOT NULL,
    `Press` int NOT NULL,
    `Hum` tinyint NOT NULL,
    `Co` int NOT NULL,

    /* KEY */
    PRIMARY KEY(id),
    FOREIGN KEY(esp_ssid) REFERENCES config(Ssid)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE `protocol_3` (
    `id` int NOT NULL AUTO_INCREMENT,
    `esp_ssid` int NOT NULL, /*CHECK*/
    /* DATA */
    `Batt_level` tinyint NOT NULL,
    `Timestamp` timestamp DEFAULT CURRENT_TIMESTAMP,
    `Temp` tinyint NOT NULL,
    `Press` int NOT NULL,
    `Hum` tinyint NOT NULL,
    `Co` int NOT NULL,
    `RMS` int NOT NULL,

    /* KEY */
    PRIMARY KEY(id),
    FOREIGN KEY(esp_ssid) REFERENCES config(Ssid)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE `protocol_4` (
    `id` int NOT NULL AUTO_INCREMENT,
    `esp_ssid` int NOT NULL, /*CHECK*/
    /* DATA */
    `Batt_level` tinyint NOT NULL,
    `Timestamp` timestamp DEFAULT CURRENT_TIMESTAMP,
    `Temp` tinyint NOT NULL,
    `Press` int NOT NULL,
    `Hum` tinyint NOT NULL,
    `Co` int NOT NULL,
    `RMS` int NOT NULL,
    `Amp_x` int NOT NULL,
    `Frec_x` int NOT NULL,
    `Amp_y` int NOT NULL,
    `Frec_y` int NOT NULL,
    `Amp_z` int NOT NULL,
    `Frec_z` int NOT NULL,

    /* KEY */
    PRIMARY KEY(id),
    FOREIGN KEY(esp_ssid) REFERENCES config(Ssid)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

/* https://sebhastian.com/mysql-array/ */
CREATE TABLE `protocol_5` (
    `id` int NOT NULL AUTO_INCREMENT,
    `esp_ssid` int NOT NULL, /*CHECK*/
    /* DATA */
    `Batt_level` tinyint NOT NULL,
    `Timestamp` timestamp DEFAULT CURRENT_TIMESTAMP,
    `Temp` tinyint NOT NULL,
    `Press` int NOT NULL,
    `Hum` tinyint NOT NULL,
    `Co` int NOT NULL,
    `Acc_X` JSON NOT NULL,
    `Acc_y` JSON NOT NULL,
    `Acc_z` JSON NOT NULL,


    /* KEY */
    PRIMARY KEY(id),
    FOREIGN KEY(esp_ssid) REFERENCES config(Ssid)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
