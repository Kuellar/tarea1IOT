import os
import pygatt
from dotenv import load_dotenv
load_dotenv()


def searchConnectionBT(self):
    BLE_SCAN = os.getenv('BLE_SCAN')
    BLE_SCAN_TIMEOUT = os.getenv('BLE_SCAN_TIMEOUT')

    self.consoleLog("Searching devices...")
    devices = []

    # Conexion con pygatt
    if BLE_SCAN:
        adapter = pygatt.backends.GATTToolBackend()
        try:
            adapter.start()
            devices = adapter.scan(
                timeout=int(BLE_SCAN_TIMEOUT),
                run_as_root=True,
            )
        finally:
            adapter.stop()

    devices.append({
        'address': '01:23:45:67:89:ab',
        'name': 'ESP_32_1',
    })
    devices.append({
        'address': '01:23:45:67:89:ac',
        'name': 'ESP_32_2',
    })

    self.selectBTComboBox.clear()
    for device in devices:
        self.selectBTComboBox.addItem(
            f"{device['name']} - {device['address']}")
    self.consoleLog(f"{len(devices)} BLE devices found")


def connectBT(self):
    device = self.selectBTComboBox.currentText()
    device_address = device[-17:]

    adapter = pygatt.backends.GATTToolBackend()

    self.consoleLog(f" Connecting to {device} ...")
    try:
        adapter.start()
        self.device = adapter.connect(device_address)
        self.consoleLog(f" Connected to {device} ...")
    finally:
        adapter.stop()
        self.device = None
        self.consoleLog(f" Connection closed")

    value = "a1e8f5b1-696b-4e4c-87c6-69dfe0b0093"
