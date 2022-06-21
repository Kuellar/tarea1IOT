import socket
import time

HOST = '127.0.0.1'
PORT = 5001

# UDP: SOCK_DGRAM
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def get_protocol_4():
    res = b''

    # ID DEVICE
    res += (1).to_bytes(2, byteorder='little')
    # MAC
    res += b'\x00\x1e\xc2\x9e\x28\xeb'
    # ID PROTOCOL
    res += (4).to_bytes(1, byteorder='little')
    # leng MSG
    res += (19216).to_bytes(2, byteorder='little')
    # DATA 1
    res += (1).to_bytes(1, byteorder='little')
    # DATA 2
    res += (0).to_bytes(1, byteorder='little')
    # DATA 3
    res += (int(time.time())).to_bytes(4, byteorder='little')
    # DATA 4
    res += (12).to_bytes(1, byteorder='little')
    # DATA 5
    res += (1354).to_bytes(4, byteorder='little')
    # DATA 6
    res += (23).to_bytes(1, byteorder='little')
    # DATA 7
    res += (4321).to_bytes(4, byteorder='little')
    # DATA 8
    res += (2948752893659).to_bytes(6400, byteorder='little')
    # DATA 9
    res += (3468467834562).to_bytes(6400, byteorder='little')
    # DATA 10
    res += (2456867456783).to_bytes(6400, byteorder='little')

    #print(res)
    return res

while True:
    s.sendto(get_protocol_4(), (HOST, PORT))
    time.sleep(0.5)
