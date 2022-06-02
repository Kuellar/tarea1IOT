from sqlalchemy import (DateTime, Column, Integer, SmallInteger, ForeignKey, JSON)
from sqlalchemy.orm import declarative_base


base = declarative_base()
class Config(base):
    __tablename__ = 'config'

    Status = Column(SmallInteger)
    ID_Protocol = Column(SmallInteger)
    BMI270_sampling = Column(Integer)
    BMI270_Acc_Sensibility = Column(Integer)
    BMI270_Gyro_Sensibility = Column(Integer)
    BME688_Sampling = Column(Integer)
    Discontinuous_Time = Column(Integer)
    Port_TCP = Column(Integer)
    Port_UDP = Column(Integer)
    Host_Ip_Addr = Column(Integer)
    Ssid = Column(Integer, primary_key=True)
    Pass = Column(Integer)


class Protocol0(base):
    __tablename__ = 'protocol_0'

    id = Column(Integer, primary_key=True)
    esp_ssid = Column(Integer, ForeignKey("config.Ssid"), primary_key=True)
    ok = Column(SmallInteger)


class Protocol1(base):
    __tablename__ = 'protocol_1'

    id = Column(Integer, primary_key=True)
    esp_ssid = Column(Integer, ForeignKey("config.Ssid"), primary_key=True)
    Batt_level = Column(SmallInteger)
    Timestamp = Column(DateTime)


class Protocol2(base):
    __tablename__ = 'protocol_2'

    id = Column(Integer, primary_key=True)
    esp_ssid = Column(Integer, ForeignKey("config.Ssid"), primary_key=True)
    Batt_level = Column(SmallInteger)
    Timestamp = Column(DateTime)
    Temp = Column(SmallInteger)
    Press = Column(Integer)
    Hum = Column(SmallInteger)
    Co = Column(Integer)


class Protocol3(base):
    __tablename__ = 'protocol_3'

    id = Column(Integer, primary_key=True)
    esp_ssid = Column(Integer, ForeignKey("config.Ssid"), primary_key=True)
    Batt_level = Column(SmallInteger)
    Timestamp = Column(DateTime)
    Temp = Column(SmallInteger)
    Press = Column(Integer)
    Hum = Column(SmallInteger)
    Co = Column(Integer)
    RMS = Column(Integer)


class Protocol4(base):
    __tablename__ = 'protocol_4'

    id = Column(Integer, primary_key=True)
    esp_ssid = Column(Integer, ForeignKey("config.Ssid"), primary_key=True)
    Batt_level = Column(SmallInteger)
    Timestamp = Column(DateTime)
    Temp = Column(SmallInteger)
    Press = Column(Integer)
    Hum = Column(SmallInteger)
    Co = Column(Integer)
    RMS = Column(Integer)
    Amp_x = Column(Integer)
    Frec_x = Column(Integer)
    Amp_y = Column(Integer)
    Frec_y = Column(Integer)
    Amp_z = Column(Integer)
    Frec_z = Column(Integer)


class Protocol5(base):
    __tablename__ = 'protocol_5'

    id = Column(Integer, primary_key=True)
    esp_ssid = Column(Integer, ForeignKey("config.Ssid"), primary_key=True)
    Batt_level = Column(SmallInteger)
    Timestamp = Column(DateTime)
    Temp = Column(SmallInteger)
    Press = Column(Integer)
    Hum = Column(SmallInteger)
    Co = Column(Integer)
    Acc_X = Column(JSON)  # ARRAY
    Acc_y = Column(JSON)  # ARRAY
    Acc_z = Column(JSON)  # ARRAY
