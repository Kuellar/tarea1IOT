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
            # self.setProtocol(ID_Protocol)
            data = (session
                .query(self.protocol_list[ID_Protocol])
                .filter(self.protocol_list[ID_Protocol].mac == self.mac)
                .order_by(self.protocol_list[ID_Protocol].Timestamp.desc())
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

def add_data(data):
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
            print("ERROR")
            session.close()
            return False

def save_data_1(header, body):
    db_data = Protocol1(
        mac = header["mac"],
        Batt_level = body["batt_level"],
        Timestamp = body["timestamp"],
    )
    return add_data(db_data)

def save_data_2(header, body):
    db_data = Protocol2(
        mac = header["mac"],
        Batt_level = body["batt_level"],
        Timestamp = body["timestamp"],
        Temp =  body["temp"],
        Press =  body["press"],
        Hum =  body["hum"],
        Co =  body["co"],
    )
    return add_data(db_data)

def save_data_3(header, body):
    db_data = Protocol3(
        mac = header["mac"],
        Batt_level = body["batt_level"],
        Timestamp = body["timestamp"],
        Temp =  body["temp"],
        Press =  body["press"],
        Hum =  body["hum"],
        Co =  body["co"],
        RMS = body["rms"],
    )
    return add_data(db_data)

def save_data_4(header, body):
    db_data = Protocol4(
        mac = header["mac"],
        Batt_level = body["batt_level"],
        Timestamp = body["timestamp"],
        Temp =  body["temp"],
        Press =  body["press"],
        Hum =  body["hum"],
        Co =  body["co"],
        RMS = body["rms"],
        Amp_x = body["amp_x"],
        Frec_x = body["frec_x"],
        Amp_y = body["amp_y"],
        Frec_y = body["frec_y"],
        Amp_z = body["amp_z"],
        Frec_z = body["frec_z"],
    )
    return add_data(db_data)

def drop_tables():
    engine = start_connection()
    if not engine:
        return
    SessionFactory = sessionmaker(engine)
    with SessionFactory() as session:
        try:
            total_data = session.query(Protocol0).delete()
            total_data += session.query(Protocol1).delete()
            total_data += session.query(Protocol2).delete()
            total_data += session.query(Protocol3).delete()
            total_data += session.query(Protocol4).delete()
            total_data += session.query(Protocol5).delete()
            session.commit()
            total_data += session.query(Config).delete()
            session.commit()
            print("TOTAL DATA REMOVED: ", total_data)
        except:
            session.rollback()
            print("FAIL")
        finally:
            session.close()
            return True

# CHECK DB
# print(get_all_configs())
# CLEAN DB
# drop_tables()