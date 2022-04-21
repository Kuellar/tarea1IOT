#!/usr/bin/env python3
import sys
import socket
from datetime import datetime
import struct
import threading

HOST = '192.168.1.8'
PORT = 5001
FILENAME = 'udp_log.csv'
HEADER_CSV = 'ID Device,MAC,ID Protocol,leng msg,Val: 1,Batt_level,Timestamp,Temp,Press,Hum,Co,Acc_x,Acc_y,Acc_z'
# LENGTH
ID_DEVICE = 2
MAC = 6
ID_PROTOCOL = 1
LENG_MSG = 2
DATA_1 = 1
DATA_2 = 1
DATA_3 = 4
DATA_4 = 1
DATA_5 = 4
DATA_6 = 1
DATA_7 = 4
DATA_8 = 6400
DATA_9 = 6400
DATA_10 = 6400

BYTE_ORDER = 'little'

STOP = False

def set_file(file):
    try:
        f = open(file, 'r')
    except:
        f = open(file, 'w')
        f.write(HEADER_CSV+'\n')
    else:
        check = f.read(len(HEADER_CSV)) == HEADER_CSV
        if not check:
            f = open(file, 'w')
            f.write(HEADER_CSV+'\n')
    finally:
        f.close()

def write_line(file, data):
    protocol = int.from_bytes(data[8:8+ID_PROTOCOL], byteorder=BYTE_ORDER)

    if protocol != 4:
        print("ERROR: INVALID DATA - THE PROTOCOL IS NOT VALID")
    # print("Data received: PROTOCOL ", protocol)

    n = 0
    ###### HEADER ######
    # ID Device - 2 bytes
    file.write(str(int.from_bytes(data[n:n+ID_DEVICE], byteorder=BYTE_ORDER)))
    n += ID_DEVICE

    # MAC - 6 bytes
    file.write(','+str(data[n:n+1].hex()))
    file.write(':'+str(data[n+1:n+2].hex()))
    file.write(':'+str(data[n+2:n+3].hex()))
    file.write(':'+str(data[n+3:n+4].hex()))
    file.write(':'+str(data[n+4:n+5].hex()))
    file.write(':'+str(data[n+5:n+6].hex()))
    n += MAC

    # ID Protocol - 1 bytes
    file.write(','+str(protocol))
    n += ID_PROTOCOL

    # leng msg - 2 bytes
    file.write(','+str(int.from_bytes(data[n:n+LENG_MSG], byteorder=BYTE_ORDER)))
    n += LENG_MSG

    ###### DATA ######

    # Val: 1 - 1 bytes
    file.write(','+str(int.from_bytes(data[n:n+DATA_1], byteorder=BYTE_ORDER)))
    n += DATA_1

    # Batt_level - 1 bytes
    file.write(','+str(int.from_bytes(data[n:n+DATA_2], byteorder=BYTE_ORDER)))
    n += DATA_2

    # Timestamp - 4 bytes
    now = datetime.now()
    file.write(','+now.strftime('%d/%m/%Y %H:%M:%S'))
    n += DATA_3

    # Temp - 1 bytes
    file.write(','+str(int.from_bytes(data[n:n+DATA_4], byteorder=BYTE_ORDER)))
    n += DATA_4

    # Press - 4 bytes
    # file.write(','+str(int.from_bytes(data[n:n+DATA_5], byteorder=BYTE_ORDER)))
    file.write(','+str(struct.unpack('f',data[n:n+DATA_5])[0]))
    n += DATA_5

    # Hum - 1 bytes
    file.write(','+str(int.from_bytes(data[n:n+DATA_6], byteorder=BYTE_ORDER)))
    n += DATA_6

    # Co - 4 bytes
    # file.write(','+str(int.from_bytes(data[n:n+DATA_7], byteorder=BYTE_ORDER)))
    file.write(','+str(struct.unpack('f',data[n:n+DATA_7])[0]))
    n += DATA_7

    # Acc_x
    file.write(',')
    for i in range(1600):
        file.write(str(struct.unpack('f',data[n+i*4:n+(i+1)*4])[0])+'_')
    n += DATA_8

    # Acc_y
    file.write(',')
    for i in range(1600):
        file.write(str(struct.unpack('f',data[n+i*4:n+(i+1)*4])[0])+'_')
    n += DATA_9

    # Acc_z
    file.write(',')
    for i in range(1600):
        file.write(str(struct.unpack('f',data[n+i*4:n+(i+1)*4])[0])+'_')
    n += DATA_10

    file.write('\n')

def connection(port, file, host):
    global STOP
    while True:
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            s.bind((host, port))
            while True:
                if STOP:
                    return
                
                data, _ = s.recvfrom(11+19216)
                if len(data == 11+19216): # Pérdida de paquetes
                    f = open(file, 'a')
                    write_line(f, data)
                    f.close()
        except Exception as e:
            print("Exception: ", e)
            print("Rebooting server...")

def server(port=PORT, file=FILENAME, host=HOST):
    global STOP
    set_file(file)
    print("HOST: ", host)
    print("PORT: ", port)
    print("Type stop to quit the program")
    x = threading.Thread(target=connection, args=(port,file,host,))
    x.start()
    while True:
        inp = input("")
        if inp.lower() in ["exit", "quit", "0", "stop"]:
            STOP = True
            break
    x.join()

if __name__ == '__main__':
    arg = sys.argv
    if len(arg) == 4:
        server(arg[1], arg[2], arg[3])
    elif len(arg) == 3:
        server(arg[1], arg[2])
    elif len(arg) == 2:
        server(arg[1])
    else:
        server()
