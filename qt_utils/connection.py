import os
from PyQt5.QtCore import QCoreApplication, QObject, QThread, pyqtSignal, Qt
from db.api_db import save_data_1, save_data_2, save_data_3, save_data_4, get_config
import pygatt
from dotenv import load_dotenv
load_dotenv()


class WorkerSearchConnectionBT(QObject):
    finished = pyqtSignal()
    devices = pyqtSignal([list])

    def run(self):
        BLE_SCAN_TIMEOUT = os.getenv('BLE_SCAN_TIMEOUT')
        adapter = pygatt.backends.GATTToolBackend()
        scanedDevices = []
        try:
            adapter.start()
            scanedDevices = adapter.scan(
                timeout=int(BLE_SCAN_TIMEOUT),
                run_as_root=True,
            )
            self.devices.emit(scanedDevices)
        finally:
            adapter.stop()
        self.finished.emit()


def searchConnectionBT(self):
    if self.searchBTButton.text() == "Buscando...":
        return
    _translate = QCoreApplication.translate
    self.searchBTButton.setText(_translate("Dialog", "Buscando..."))
    self.selectBTComboBox.clear()
    BLE_SCAN = os.getenv('BLE_SCAN')
    self.consoleLog("Searching devices...")

    # Conexion con pygatt
    if BLE_SCAN:
        # LAUNCH THREAD
        self.thread = QThread()
        self.worker = WorkerSearchConnectionBT()
        self.worker.moveToThread(self.thread)
        self.thread.started.connect(self.worker.run)
        self.worker.devices.connect(self.updateBTList)
        self.worker.finished.connect(self.thread.quit)
        self.worker.finished.connect(self.worker.deleteLater)
        self.thread.finished.connect(self.thread.deleteLater)
        self.thread.start()


## SUBSCRIBE HANDLER
def handle_data(handle, value):
    # print(data)
    if len(value) < 9:
        return
    protocol = int.from_bytes(value[0:1], byteorder="big")
    if protocol not in [0, 1, 2, 3, 4, 5]:
        return
    status = int.from_bytes(value[1:2], byteorder="big")
    if status not in [0, 20, 21, 22, 23, 30, 31]:
        return
    mac = int.from_bytes(value[2:8], byteorder="big")
    leng_msg = int.from_bytes(value[8:9], byteorder="big")
    header = {
        "protocol": protocol,
        "status": status,
        "mac": mac,
        "leng_msg": leng_msg
     }
    print(header)
    if header["protocol"] == 1:
        save_data_1(header, value[9:])
    if header["protocol"] == 2:
        save_data_2(header, value[9:])
    if header["protocol"] == 3:
        save_data_3(header, value[9:])
    if header["protocol"] == 4:
        save_data_4(header, value[9:])


## SUBSCRIBE
def connectBT(self):
    device = self.selectBTComboBox.currentText()
    if not device:
        self.consoleLog("ERROR: Device not selected")
        return

    device_address = device[-17:]
    self.mac = int(device_address.replace(":",""), 16) - 2

    old_config = get_config(self.mac)
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
        self.setProtocol(self, old_config[0].ID_Protocol)
        self.started_monitoring = False
        self.selectVariableBox.clear()
        

        if old_config[0].Status not in [0, 20]:
            self.AccSamplingBox.setDisabled(True)
            self.AccSensibilityBox.setDisabled(True)
            self.GyroSensibilityBox.setDisabled(True)
            self.BME688SamplingBox.setDisabled(True)
            self.discontinuosTimeBox.setDisabled(True)
            self.portTCPBox.setDisabled(True)
            self.portUDPBox.setDisabled(True)
            self.hostIPAddrBox.setDisabled(True)
            self.ssidBox.setDisabled(True)
            self.passBox.setDisabled(True)
        else:
            self.AccSamplingBox.setDisabled(False)
            self.AccSensibilityBox.setDisabled(False)
            self.GyroSensibilityBox.setDisabled(False)
            self.BME688SamplingBox.setDisabled(False)
            self.discontinuosTimeBox.setDisabled(False)
            self.portTCPBox.setDisabled(False)
            self.portUDPBox.setDisabled(False)
            self.hostIPAddrBox.setDisabled(False)
            self.ssidBox.setDisabled(False)
            self.passBox.setDisabled(False)
    
    else:
        # Guardamos el protocolo 0 si no hemos registrado el ESP32
        self.setProtocol(self, 0)
        self.started_monitoring = False
        self.selectVariableBox.clear()
        
    adapter = pygatt.backends.GATTToolBackend()

    self.consoleLog(f" Connecting to {device} ...")
    try:
        adapter.start()
        self.device = adapter.connect(device_address)
        self.consoleLog(f" Connected to {device} ...")
        self.device.subscribe(self.deviceUUID, callback=handle_data, wait_for_response=False)
        self.label_statusESP.setText("Conectado")
    except Exception as e:
        adapter.stop()
        self.device = None
        self.consoleLog(f" Connection closed - {e}")
        self.label_statusESP.setText("Desconectado")
