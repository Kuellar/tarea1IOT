import os
import socket
from time import sleep
from PyQt5.QtCore import QObject, QThread, pyqtSignal
from dotenv import load_dotenv
load_dotenv()

class WorkerUdpServer(QObject):
    finished = pyqtSignal()

    def run(self):
        HOST = os.getenv('HOST')
        PORT = int(os.getenv('UPD_PORT'))
        print("INIT UDP PORT: ", PORT)
        while True:
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                s.bind((HOST, PORT))
                while True:
                    data, _ = s.recvfrom(11+19216)
                    print(data)
                    sleep(1)
            except Exception as e:
                print("UDP: Exception - ", e)
                print("UDP: Rebooting serve...")
        print("UDP: ERROR")

def initUdpServer(self):
    self.UDPthread = QThread()
    self.UDPworker = WorkerUdpServer()
    self.UDPworker.moveToThread(self.UDPthread)
    self.UDPthread.started.connect(self.UDPworker.run)
    self.UDPworker.finished.connect(self.UDPthread.quit)
    self.UDPworker.finished.connect(self.UDPworker.deleteLater)
    self.UDPthread.finished.connect(self.UDPthread.deleteLater)
    self.UDPthread.start()
