#!/usr/bin/env python3
import socket
import time

HOST = '127.0.0.1'
PORT = 5001

def get_protocol_0(prot=0, lenmsg=6):
    res = b''

    # ID DEVICE
    res += (1).to_bytes(2, byteorder='little')
    # MAC
    res += b'\x00\x1e\xc2\x9e\x28\xeb'
    # ID PROTOCOL
    res += (prot).to_bytes(1, byteorder='little')
    # leng MSG
    res += (lenmsg).to_bytes(2, byteorder='little')
    # DATA 1
    res += (1).to_bytes(1, byteorder='little')
    # DATA 2
    res += (0).to_bytes(1, byteorder='little')
    # DATA 3
    res += (int(time.time())).to_bytes(4, byteorder='little')

    # print(res)
    return res

def get_protocol_1(prot=1, lenmsg=16):
    res = b''
    res += get_protocol_0(prot, lenmsg)

    # DATA 4
    res += (12).to_bytes(1, byteorder='little')
    # DATA 5
    res += (1354).to_bytes(4, byteorder='little')
    # DATA 6
    res += (23).to_bytes(1, byteorder='little')
    # DATA 7
    res += (4321).to_bytes(4, byteorder='little')

    # print(res)
    return res

def get_protocol_2(prot=2, lenmsg=20):
    res = b''
    res += get_protocol_1(prot, lenmsg)

    # DATA 8
    res += (7654).to_bytes(4, byteorder='little')

    # print(res)
    return res

def get_protocol_3():
    res = b''
    res += get_protocol_2(3, 44)

    # DATA 9
    res += (1234).to_bytes(4, byteorder='little')
    # DATA 10
    res += (5678).to_bytes(4, byteorder='little')
    # DATA 11
    res += (9123).to_bytes(4, byteorder='little')
    # DATA 12
    res += (4567).to_bytes(4, byteorder='little')
    # DATA 13
    res += (7890).to_bytes(4, byteorder='little')
    # DATA 14
    res += (47343).to_bytes(4, byteorder='little')

    print(res)
    return res


# TCP: SOCK_STREAM
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))

print('Client connected.')

s.sendall(get_protocol_0())
data = s.recv(1)
print("Data received from server: ", data.decode('utf-8'))
s.sendall(get_protocol_1())
data = s.recv(1)
print("Data received from server: ", data.decode('utf-8'))
s.sendall(get_protocol_2())
data = s.recv(1)
print("Data received from server: ", data.decode('utf-8'))
s.sendall(get_protocol_3())
data = s.recv(1)
print("Data received from server: ", data.decode('utf-8'))
s.close()