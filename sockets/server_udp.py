import os
import socket
from PyQt5.QtCore import QObject, QThread, pyqtSignal
from sqlalchemy import true
from db.api_db import save_data_1, save_data_2, save_data_3, save_data_4, get_config
from db.utils import translateData
from dotenv import load_dotenv
load_dotenv()

class WorkerUdpServer(QObject):
    finished = pyqtSignal()
    connected = pyqtSignal(bool)

    def run(self):
        HOST = os.getenv('HOST')
        PORT = int(os.getenv('UPD_PORT'))
        print("INIT UDP PORT: ", PORT)
        while True:
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                s.bind((HOST, PORT))
                while True:
                    data, adr = s.recvfrom(7000)
                    self.connected.emit(True)
                    header, body = translateData(data)
                    if header and body:
                        print(header, body)
                        if header["protocol"] == 1 and len(body) == 5:
                            save_data_1(header, body)
                        if header["protocol"] == 2 and len(body) == 15:
                            save_data_2(header, body)
                        if header["protocol"] == 3 and len(body) == 19:
                            save_data_3(header, body)
                        if header["protocol"] == 4 and len(body) == 43:
                            save_data_4(header, body)

                        # SEND DATA
                        config = get_config(header["mac"])[0]
                        new_status = config.Status.to_bytes(1, 'big')
                        new_protocol = config.ID_Protocol.to_bytes(1, 'big')
                        s.sendto(new_status+new_protocol, adr)
                    else:
                        # ERROR: Send status 0 - protocol - 0
                        s.sendto(b'\x00\x00', adr)
            except Exception as e:
                print("UDP: Exception - ", e)
                print("UDP: Rebooting serve...")
        print("UDP: ERROR")

def initUdpServer(self):
    self.UDPthread = QThread()
    self.UDPworker = WorkerUdpServer()
    self.UDPworker.moveToThread(self.UDPthread)
    self.UDPthread.started.connect(self.UDPworker.run)
    self.UDPworker.connected.connect(self.setConnected)
    self.UDPworker.finished.connect(self.UDPthread.quit)
    self.UDPworker.finished.connect(self.UDPworker.deleteLater)
    self.UDPthread.finished.connect(self.UDPthread.deleteLater)
    self.UDPthread.start()
