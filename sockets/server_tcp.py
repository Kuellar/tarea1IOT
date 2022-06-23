import os
import socket
from PyQt5.QtCore import QObject, QThread, pyqtSignal
from dotenv import load_dotenv
from db.api_db import save_data_1, save_data_2, save_data_3, save_data_4, get_config
from db.utils import translateData
load_dotenv()

class WorkerTcpServer(QObject):
    finished = pyqtSignal()
    connected = pyqtSignal(int)

    def run(self):
        HOST = os.getenv('HOST')
        PORT = int(os.getenv('TCP_PORT'))
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.bind((HOST, PORT))
        print("INIT TCP PORT: ", PORT)
        s.listen(1)  # 1 accepted connections
        while True:
            try:
                conn, addr = s.accept()  # waits for a new connection
                self.connected.emit(True)
                print('TCP: Client connected: ', addr)
                while True:
                    # RECEIVE DATA
                    data = conn.recv(7000)
                    if not data:
                        break
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
                        conn.sendall(new_status+new_protocol)
                    else:
                        # ERROR: Send status 0 - protocol - 0
                        conn.sendall(b'\x00\x00')
                conn.close()
                self.connected.emit(False)
                print("TCP: Client disconected: ", addr)
            except Exception as e:
                print("TCP: Exception - ", e)
        print("TCP: ERROR")

def initTcpServer(self):
    self.TCPthread = QThread()
    self.TCPworker = WorkerTcpServer()
    self.TCPworker.moveToThread(self.TCPthread)
    self.TCPthread.started.connect(self.TCPworker.run)
    self.TCPworker.connected.connect(self.setConnected)
    self.TCPworker.finished.connect(self.TCPthread.quit)
    self.TCPworker.finished.connect(self.TCPworker.deleteLater)
    self.TCPthread.finished.connect(self.TCPthread.deleteLater)
    self.TCPthread.start()
