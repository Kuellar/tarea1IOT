import os
from time import sleep
from binascii import hexlify
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

    self.selectBTComboBox.clear()
    for device in devices:
        self.selectBTComboBox.addItem(
            f"{device['name']} - {device['address']}")
    self.consoleLog(f"{len(devices)} BLE devices found")


## SUBSCRIBE HANDLER
def handle_data(handle, value):
    print("Received data: %s" % hexlify(value))


## SUBSCRIBE
def connectBT(self):
    self.consoleLog(f"DEBUG")
    device = self.selectBTComboBox.currentText()
    device_address = device[-17:]

    adapter = pygatt.backends.GATTToolBackend()

    self.consoleLog(f" Connecting to {device} ...")
    try:
        adapter.start()
        self.device = adapter.connect(device_address)
        self.consoleLog(f" Connected to {device} ...")
        value = "0000ff01-0000-1000-8000-00805F9B34FB"
        self.device.subscribe(value, callback=handle_data, wait_for_response=False)
        self.consoleLog(f" Value {value} ...")
    except Exception as e:
        adapter.stop()
        self.device = None
        self.consoleLog(f" Connection closed - {e}")
