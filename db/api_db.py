import os
from sqlalchemy import create_engine, MetaData, desc
from sqlalchemy.orm import sessionmaker
from db.model import Config, Protocol0, Protocol1, Protocol2, Protocol3, Protocol4, Protocol5
from dotenv import load_dotenv
from datetime import datetime
load_dotenv()

BYTE_ORDER = 'big'

def start_connection():
    engine = None
    try:
        db_host = os.getenv('DB_HOST')
        db_user = os.getenv('DB_USER')
        db_password = os.getenv('DB_PASSWORD')
        db_database = os.getenv('DB_DATABASE')
        DATABSE_URI=f'mysql+mysqlconnector://{db_user}:{db_password}@{db_host}/{db_database}'
        engine = create_engine(DATABSE_URI)
        connection = engine.connect()
        metadata = MetaData()
    except Exception as e:
        print("ERROR DATABASE - ", e)
    return engine

def get_all_configs():
    engine = start_connection()
    if not engine:
        return
    SessionFactory = sessionmaker(engine)
    with SessionFactory() as session:
        try:
            configs = session.query(Protocol1).all()
        except:
            return {"error": ":("}
        finally:
            session.close()
            return configs

def get_config(mac):
    engine = start_connection()
    if not engine:
        return
    SessionFactory = sessionmaker(engine)
    with SessionFactory() as session:
        try:
            config = session.query(Config).filter(Config.mac == mac)
        except:
            return {"error": ":("}
        finally:
            session.close()
            return config

def get_last_data_prot(self):
    engine = start_connection()
    data = 0
    if not engine:
        return
    SessionFactory = sessionmaker(engine)
    with SessionFactory() as session:
        try:
            ID_Protocol = session.query(Config).filter(Config.mac == self.mac).first().ID_Protocol
            self.protocol  = self.protocol_list[ID_Protocol]
            data = (session
                .query(self.protocol)
                .filter(self.protocol.mac == self.mac)
                .order_by(self.protocol.Timestamp.desc())
                .first()
            )
        except:
            return {"error": ":("}
        finally:
            session.close()
            return data

def add_config(config: Config):
    engine = start_connection()
    if not engine:
        return
    SessionFactory = sessionmaker(engine)
    with SessionFactory() as session:
        try:
            session.add(config)
            session.commit()
            session.close()
            return True
        except:
            session.close()
            return False

def update_config(mac2, conf):
    engine = start_connection()
    if not engine:
        return
    SessionFactory = sessionmaker(engine)
    with SessionFactory() as session:
        try:
            cc = session.query(Config).filter_by(mac=mac2)
            cc.update(conf)
            session.commit()
        except:
            return False
        finally:
            session.close()
            return True

def add_data(data: Protocol1):
    engine = start_connection()
    if not engine:
        return
    SessionFactory = sessionmaker(engine)
    with SessionFactory() as session:
        try:
            session.add(data)
            session.commit()
            session.close()
            return True
        except:
            session.close()
            return False

def save_data_1(header, data):
    ERR = []
    batt_level = int.from_bytes(data[0:1], byteorder=BYTE_ORDER)
    if batt_level < 0 or batt_level > 100:
        ERR.append("batt_level")
    timestamp = int.from_bytes(data[1:5], byteorder=BYTE_ORDER)
    if timestamp != 0:
        ERR.append("timestamp")
    timestamp = datetime.now()

    print(batt_level, timestamp)
    if len(ERR) != 0:
        print("ERROR: ", ERR)
        return

    db_data = Protocol1(
        mac = header["mac"],
        Batt_level = batt_level,
        Timestamp = timestamp,
    )
    add_data(db_data)

def save_data_2(header, data):
    ERR = []
    batt_level = int.from_bytes(data[0:1], byteorder=BYTE_ORDER)
    if batt_level < 0 or batt_level > 100:
        ERR.append("batt_level")
    timestamp = int.from_bytes(data[1:5], byteorder=BYTE_ORDER)
    if timestamp != 0:
        ERR.append("timestamp")
    timestamp = datetime.now()

    temp = int.from_bytes(data[5:6], byteorder=BYTE_ORDER)
    if temp < 5 or temp > 30:
        ERR.append("temp")
    press = int.from_bytes(data[6:10], byteorder=BYTE_ORDER)
    if press < 1000 or press > 1200:
        ERR.append("press")
    hum = int.from_bytes(data[10:11], byteorder=BYTE_ORDER)
    if hum < 30 or hum > 80:
        ERR.append("hum")
    co = int.from_bytes(data[11:15], byteorder=BYTE_ORDER)
    if co < 30 or co > 200:
        ERR.append("co")

    print(batt_level, timestamp, temp, press, hum, co)
    if len(ERR) != 0:
        print("ERROR: ", ERR)
        return

    db_data = Protocol2(
        mac = header["mac"],
        Batt_level = batt_level,
        Timestamp = timestamp,
        Temp = temp,
        Press = press,
        Hum = hum,
        Co = co,
    )
    add_data(db_data)

def save_data_3(header, data):
    return

def save_data_4(header, data):
    return

# CHECK DB
# print(get_all_configs())