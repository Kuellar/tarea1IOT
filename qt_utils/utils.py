from PyQt5.QtCore import Qt
from db.api_db import get_config

def fillBoxes(self, old_config):
    if old_config.count() > 0:
        self.AccSamplingBox.setText(str(old_config[0].BMI270_sampling))
        self.AccSensibilityBox.setText(str(old_config[0].BMI270_Acc_Sensibility))
        self.GyroSensibilityBox.setText(str(old_config[0].BMI270_Gyro_Sensibility))
        self.BME688SamplingBox.setText(str(old_config[0].BME688_Sampling))
        self.discontinuosTimeBox.setText(str(old_config[0].Discontinuous_Time))
        self.portTCPBox.setText(str(old_config[0].Port_TCP))
        self.portUDPBox.setText(str(old_config[0].Port_UDP))
        self.hostIPAddrBox.setText(str(old_config[0].Host_Ip_Addr))
        self.ssidBox.setText(str(old_config[0].Ssid))
        self.passBox.setText(str(old_config[0].Pass))
        status_text = next(key for key, value in self.STATUS_DICT.items() if value == int(old_config[0].Status))
        status_index = self.operationModeBox.findText(status_text, Qt.MatchFixedString)
        self.operationModeBox.setCurrentIndex(status_index)
        self.operationModeSelected()
        index = self.protocolIDBox.findText(str(old_config[0].ID_Protocol), Qt.MatchFixedString)
        self.protocolIDBox.setCurrentIndex(index)

        # Guardamos el protocolo
        self.setProtocol(old_config[0].ID_Protocol)

    else:
        # Guardamos el protocolo 0 si no hemos registrado el ESP32
        self.setProtocol(0)
        self.selectVariableBox.clear()
    self.started_monitoring = False

def selectBT(self):
    device = self.selectBTComboBox.currentText()
    if not device:
        self.consoleLog("ERROR: Device not selected")
        return


    device_address = device[-17:]
    self.mac_bt = device_address
    self.mac = int(device_address.replace(":",""), 16) - 2
    old_config = get_config(self.mac)
    fillBoxes(self, old_config)
