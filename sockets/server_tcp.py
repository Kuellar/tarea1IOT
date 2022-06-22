import os
import socket
from time import sleep
from PyQt5.QtCore import QObject, QThread, pyqtSignal
from dotenv import load_dotenv
load_dotenv()

class WorkerTcpServer(QObject):
    finished = pyqtSignal()

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
                print('TCP: Client connected: ', addr)
                while True:
                    data = conn.recv(11+44)  # HEADER: 11 - DATA: 44
                    print(data)
                    if not data:
                        break
                    conn.sendall(b'0')
                conn.close()
                print("TCP: Client disconected: ", addr)
            except Exception as e:
                print("TCP: Exception - ", e)
        print("TCP: ERROR")

def initTcpServer(self):
    self.TCPthread = QThread()
    self.TCPworker = WorkerTcpServer()
    self.TCPworker.moveToThread(self.TCPthread)
    self.TCPthread.started.connect(self.TCPworker.run)
    self.TCPworker.finished.connect(self.TCPthread.quit)
    self.TCPworker.finished.connect(self.TCPworker.deleteLater)
    self.TCPthread.finished.connect(self.TCPthread.deleteLater)
    self.TCPthread.start()
