import os
from PyQt5.QtCore import QCoreApplication, QObject, QThread, pyqtSignal
from time import sleep
from binascii import hexlify
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
    print("Received data: %s" % hexlify(value))


## SUBSCRIBE
def connectBT(self):
    device = self.selectBTComboBox.currentText()
    if not device:
        self.consoleLog("ERROR: Device not selected")
        return

    device_address = device[-17:]

    adapter = pygatt.backends.GATTToolBackend()

    self.consoleLog(f" Connecting to {device} ...")
    try:
        adapter.start()
        self.device = adapter.connect(device_address)
        self.consoleLog(f" Connected to {device} ...")
        self.device.subscribe(self.deviceUUID, callback=handle_data, wait_for_response=False)
    except Exception as e:
        adapter.stop()
        self.device = None
        self.consoleLog(f" Connection closed - {e}")
