from datetime import datetime

BYTE_ORDER = 'big'

def translateData(data):
    if len(data) < 9:
        return False, False

    protocol = int.from_bytes(data[0:1], byteorder="big")
    if protocol not in [0, 1, 2, 3, 4, 5]:
        print("ERROR: Invalid protocol", protocol)
        return False, False

    status = int.from_bytes(data[1:2], byteorder="big")
    if status not in [0, 20, 21, 22, 23, 30, 31]:
        print("ERROR: Invalid status", status)
        return False, False

    mac = int.from_bytes(data[2:8], byteorder="big")
    leng_msg = int.from_bytes(data[8:9], byteorder="big")
    if leng_msg not in [0, 1, 6, 16, 20, 44]:
        print("ERROR: Invalid leng msg", leng_msg)
        return False, False

    header = {
        "protocol": protocol,
        "status": status,
        "mac": mac,
        "leng_msg": leng_msg
    }

    bodyData = data[9:]
    if len(bodyData) not in [1, 5, 15, 19, 43]:
        print("ERROR: Invalid leng body", len(bodyData))
        return False, False

    body = {}
    ERR = []
    if protocol in [1, 2, 3, 4, 5]:
        batt_level = int.from_bytes(bodyData[0:1], byteorder=BYTE_ORDER)
        if batt_level < 0 or batt_level > 100:
            ERR.append("batt_level")
        body["batt_level"] = batt_level
        timestamp = int.from_bytes(bodyData[1:5], byteorder=BYTE_ORDER)
        if timestamp != 0:
            ERR.append("timestamp")
        body["timestamp"] = datetime.now()

    if protocol in [2, 3, 4, 5]:
        temp = int.from_bytes(bodyData[5:6], byteorder=BYTE_ORDER)
        if temp < 5 or temp > 30:
            ERR.append("temp")
        body["temp"] = temp
        press = int.from_bytes(bodyData[6:10], byteorder=BYTE_ORDER)
        if press < 1000 or press > 1200:
            ERR.append("press")
        body["press"] = press
        hum = int.from_bytes(bodyData[10:11], byteorder=BYTE_ORDER)
        if hum < 30 or hum > 80:
            ERR.append("hum")
        body["hum"] = hum
        co = int.from_bytes(bodyData[11:15], byteorder=BYTE_ORDER)
        if co < 30 or co > 200:
            ERR.append("co")
        body["co"] = co

    if protocol in [3, 4]:
        rms = int.from_bytes(bodyData[15:19], byteorder=BYTE_ORDER) / 100000000
        if rms < 0.01 or rms > 0.3:
            ERR.append("rms")
        body["rms"] = rms

    if protocol in [4]:
        amp_x = int.from_bytes(bodyData[19:23], byteorder=BYTE_ORDER) / 100000000
        if amp_x < 0.0059 or amp_x > 0.12:
            ERR.append("amp_x")
        body["amp_x"] = amp_x
        frec_x = int.from_bytes(bodyData[23:27], byteorder=BYTE_ORDER) / 1000000
        if frec_x < 29 or frec_x > 31:
            ERR.append("frec_x")
        body["frec_x"] = frec_x
        amp_y = int.from_bytes(bodyData[27:31], byteorder=BYTE_ORDER) / 100000000
        if amp_y < 0.0041 or amp_y > 0.11:
            ERR.append("amp_y")
        body["amp_y"] = amp_y
        frec_y = int.from_bytes(bodyData[31:35], byteorder=BYTE_ORDER) / 1000000
        if frec_y < 59 or frec_y > 61:
            ERR.append("frec_y")
        body["frec_y"] = frec_y
        amp_z = int.from_bytes(bodyData[35:39], byteorder=BYTE_ORDER) / 100000000
        if amp_z < 0.008 or amp_z > 0.15:
            ERR.append("amp_z")
        body["amp_z"] = amp_z
        frec_z = int.from_bytes(bodyData[39:43], byteorder=BYTE_ORDER) / 1000000
        if frec_z < 89 or frec_z > 91:
            ERR.append("frec_z")
        body["frec_z"] = frec_z

    if len(ERR) != 0:
        print("ERROR: ", ERR)
        return False, False

    return header, body