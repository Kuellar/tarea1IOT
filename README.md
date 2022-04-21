# Tarea 1 - Diseño de Sistemas de Internet de las Cosas CC5326-1

Profesor: Luciano Radrigan F

Estudiantes: Ignacio Cuellar - Lucas Oyarzun

## How to run

Para ejecutar los servidores tcp y udp incluidos en server/ se deben llamar los siguientes comandos:

# SERVER - Raspberry

## TCP

### How to run

> `$ python3 server/server_tcp.py`

> `$ python3 server/server_tcp.py [port]`

> `$ python3 server/server_tcp.py [port] [filename]`

> `$ python3 server/server_tcp.py [port] [filename] [host]`

#### Default values:

|          | value       |
| -------- | ----------- |
| PORT     | 5001        |
| FILENAME | tcp_log.csv |
| HOST     | 192.168.1.8 |

## UDP

### How to run

> `$ python3 server/server_udp.py`

> `$ python3 server/server_udp.py [port]`

> `$ python3 server/server_udp.py [port] [filename]`

> `$ python3 server/server_udp.py [port] [filename] [host]`

Para ejecutar los clientes tcp y udp incluidos en tdp_client_S1/ y udp_client_S2/ se debe seleccionar el SSID
y la contraseña de la red wifi a la cual se desea conectar, para esto, cambiar las variables globales de TCP_client_S1/main/wifi-connection.c y UDP_client_S2/main/wifi-connection.c UDP_HOST (poniendo el host en hexadecimal) y UDP_PORT (Puerto al cual conectarse)

Luego se deben llamar los siguientes comandos desde el shell de la ESP32:

# TCP

> `$ cd tcp_client_S1`

> `$ idf.py set-target esp32`

> `$ idf.py build`

> `$ idf.py -p COM3 flash`

# UDPP

> `$ cd udp_client_S1`

> `$ idf.py set-target esp32`

> `$ idf.py build`

> `$ idf.py -p COM3 flash`

# Optional

> `$ idf.py -p COM3 monitor`

## Table of contents

- [Raspberry](https://github.com/Kuellar/tarea1IOT/tree/main/server)
- [ESP32 S1 - TCP client](https://github.com/Kuellar/tarea1IOT/tree/main/tcp_client_S1)
- [ESP32 S2 - UPD client](https://github.com/Kuellar/tarea1IOT/tree/main/udp_client_S2)

## Authors

Ignacio Cuellar [@Kuellar](https://github.com/Kuellar)
Lucas Oyarzun [@LucasOyarzun](https://github.com/LucasOyarzun)

## Acknowledgments

- [Get Started ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)
- [Get Started WROVER kit](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-wrover-kit.html)
- [Examples ESP32](https://github.com/espressif/esp-idf/tree/master/examples)
- [Instalation Raspbian](www.youtube.com/watch?v=cxhctYvQomY)
