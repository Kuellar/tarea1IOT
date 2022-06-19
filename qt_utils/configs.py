import pygatt
from db.model import Config
from db.api_db import add_config, update_config, get_config
STATUS_DICT = {
        "Configuracion por Bluetooth": 0,
        "Configuracion via TCP en BD": 20,
        "Conexion TCP continua": 21,
        "Conexion TCP discontinua": 22,
        "Conexion UDP": 23,
        "BLE continua": 30,
        "BLE discontinua": 31
}


def saveConfiguration(self):
    # SEND CONFIG TO ESP

    val_boxes = [
        int(self.AccSamplingBox.toPlainText()).to_bytes(4, 'big'),
        int(self.AccSensibilityBox.toPlainText()).to_bytes(4, 'big'),
        int(self.GyroSensibilityBox.toPlainText()).to_bytes(4, 'big'),
        int(self.BME688SamplingBox.toPlainText()).to_bytes(4, 'big'),
        int(self.discontinuosTimeBox.toPlainText()).to_bytes(4, 'big'),
        int(self.portTCPBox.toPlainText()).to_bytes(4, 'big'),
        int(self.portUDPBox.toPlainText()).to_bytes(4, 'big'),
        int(self.hostIPAddrBox.toPlainText()).to_bytes(4, 'big'),
        int(self.ssidBox.toPlainText()).to_bytes(4, 'big'),
        int(self.passBox.toPlainText()).to_bytes(4, 'big'),
    ]

    for idx, val_box in enumerate(val_boxes):
        esp_data = (
            bytearray((3).to_bytes(1, 'big')) +
            bytearray((idx+3).to_bytes(1, 'big')) +
            bytearray(val_box)
        )
        self.device.char_write(self.deviceUUID, esp_data)

    # SAVE DATA IN DB
    self.consoleLog("Saving configuration...")
    lastConfig = get_config(self.mac)
    if lastConfig.count() == 1:
        try:
            new_config = {
                "BMI270_sampling": int(self.AccSamplingBox.toPlainText()),
                "BMI270_Acc_Sensibility": int(self.AccSensibilityBox.toPlainText()),
                "BMI270_Gyro_Sensibility": int(self.GyroSensibilityBox.toPlainText()),
                "BME688_Sampling": int(self.BME688SamplingBox.toPlainText()),
                "Discontinuous_Time": int(self.discontinuosTimeBox.toPlainText()),
                "Port_TCP": int(self.portTCPBox.toPlainText()),
                "Port_UDP": int(self.portUDPBox.toPlainText()),
                "Host_Ip_Addr": int(self.hostIPAddrBox.toPlainText()),
                "Ssid": int(self.ssidBox.toPlainText()),
                "Pass": int(self.passBox.toPlainText()),
            }
        except:
            self.consoleLog("Wrong configuration inputs")
        if update_config(self.mac, new_config):
            self.consoleLog("Configuration saved")
        else:
            self.consoleLog("Error saving configuration")
    else:
        try:
            configs = Config(
                mac = self.mac,
                Status = 0,
                ID_Protocol = 1,
                BMI270_sampling = int(self.AccSamplingBox.toPlainText()),
                BMI270_Acc_Sensibility = int(self.AccSensibilityBox.toPlainText()),
                BMI270_Gyro_Sensibility = int(self.GyroSensibilityBox.toPlainText()),
                BME688_Sampling = int(self.BME688SamplingBox.toPlainText()),
                Discontinuous_Time = int(self.discontinuosTimeBox.toPlainText()),
                Port_TCP = int(self.portTCPBox.toPlainText()),
                Port_UDP = int(self.portUDPBox.toPlainText()),
                Host_Ip_Addr = int(self.hostIPAddrBox.toPlainText()),
                Ssid = int(self.ssidBox.toPlainText()),
                Pass = int(self.passBox.toPlainText()),
            )
        except:
            self.consoleLog("Wrong configuration inputs")
        if add_config(configs):
            self.consoleLog("Configuration saved")
        else:
            self.consoleLog("Error saving configuration")

def saveStatusProtocol(self):
    # TODO: SAVE CONFIG IN DB
    try:
        Status = STATUS_DICT[self.operationModeBox.currentText()]
        ID_Protocol = int(self.protocolIDBox.currentText())
        value = "0000ff01-0000-1000-8000-00805F9B34FB"
        self.device.char_write(value, bytearray([2, Status, ID_Protocol]))
        self.consoleLog(f"Starting monitoring with Status {Status} and Protocol {ID_Protocol}")
    except Exception as e:
        self.consoleLog(f"Error - {e}")
