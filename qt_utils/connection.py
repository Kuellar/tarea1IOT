import os
import time
from PyQt5.QtCore import QCoreApplication, QObject, QThread, pyqtSignal, Qt
from db.api_db import save_data_0, save_data_1, save_data_2, save_data_3, save_data_4, get_config
from db.utils import translateData
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



## SUBSCRIBE HANDLER
def handleData(handle, value):
    header, body = translateData(value)
    if not header or not body:
        return

    saved = False
    if header["protocol"] == 0:
        saved = save_data_0(header, body)
    elif header["protocol"] == 1:
        saved = save_data_1(header, body)
    elif header["protocol"] == 2:
        saved = save_data_2(header, body)
    elif header["protocol"] == 3:
        saved = save_data_3(header, body)
    elif header["protocol"] == 4:
        saved = save_data_4(header, body)

    if not saved:
        print("BD ERROR")


class WorkerConnectBT(QObject):
    finished = pyqtSignal()

    def __init__(self, mac, parent=None):
        QThread.__init__(self, parent)
        self.mac = mac

    def run(self):
        adapter = pygatt.backends.GATTToolBackend()
        while not QThread.currentThread().isInterruptionRequested():
            try:
                adapter.start()
                device = adapter.connect(self.mac)  # device_address
                device.exchange_mtu(60)
                device.subscribe("0000ff01-0000-1000-8000-00805F9B34FB", callback=handleData, wait_for_response=False)
                print("WorkerConnectBT: Connected")
                while not QThread.currentThread().isInterruptionRequested():
                    time.sleep(4)
                    if not device.get_rssi():
                        print("WAT")
                        break

            except Exception as e:
                self.device = None
                adapter.stop()
            else:
                device.unsubscribe("0000ff01-0000-1000-8000-00805F9B34FB")
                device.disconnect(self.mac)
                adapter.stop()
                print("ELSE")
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


## SUBSCRIBE
def subscribeBT(self):
    self.consoleLog(f"Start monitoring")
    self.startMonitoring()

    # LAUNCH THREAD
    self.threadConnectBT = QThread()
    self.workerConnectBT = WorkerConnectBT(mac=self.mac_bt)
    self.workerConnectBT.moveToThread(self.threadConnectBT)
    self.threadConnectBT.started.connect(self.workerConnectBT.run)
    self.workerConnectBT.finished.connect(self.threadConnectBT.quit)
    self.workerConnectBT.finished.connect(self.workerConnectBT.deleteLater)
    self.threadConnectBT.finished.connect(self.threadConnectBT.deleteLater)
    self.threadConnectBT.start()
