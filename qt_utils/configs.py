from db.model import Config
from db.api_db import add_config
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
    self.consoleLog("Saving configuration...")
    configs = None
    try:
        configs = Config(
            Status = STATUS_DICT[self.operationModeBox.currentText()],
            ID_Protocol = int(self.protocolIDBox.currentText()),
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

