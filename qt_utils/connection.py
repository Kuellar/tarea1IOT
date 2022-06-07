import pygatt
def searchConnectionBT(self):
    # Conexion con pygatt
    # adapter = pygatt.BGAPIBackend()
    # try:
    #     adapter.start()
    #     devices = adapter.scan()
    #
    # finally:
    #     adapter.stop()

    devices = [{
        'address': '01:23:45:67:89:ab',
        'name': 'ESP_32_1',
        'rssi': 'RSSI',
        'packet_data': 'PACKET_DATA'
    }, {
        'address': '01:23:45:67:89:ac',
        'name': 'ESP_32_2',
        'rssi': 'RSSI2',
        'packet_data': 'PACKET_DATA2'
    }]

    self.selectBTComboBox.clear()
    self.consoleLog("Searching devices...")
    for device in devices:
        self.selectBTComboBox.addItem(f"{device['name']} - {device['address']}")
    self.consoleLog(f"{len(devices)} BLE devices found")


def connectBT(self):
    device = self.selectBTComboBox.currentText()
    device_address = device[-17:]

    adapter = pygatt.BGAPIBackend()

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
