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
    print("Added data TEST")
    engine = start_connection()
    if not engine:
        return
    SessionFactory = sessionmaker(engine)
    with SessionFactory() as session:
        try:
            print("Added data TEST2")
            session.add(data)
            session.commit()
            print("Added data")
            session.close()
            return True
        except:
            session.close()
            return False

def save_data_1(header, data):
    batt_level = int.from_bytes(data[0:1], byteorder="big")
    if batt_level < 0 or batt_level > 100:
        return
    timestamp = int.from_bytes(data[1:5], byteorder="big")
    if timestamp != 0:
        return
    timestamp = datetime.now()
    print(batt_level, timestamp)

    db_data = Protocol1(
        mac = header["mac"],
        Batt_level = batt_level,
        Timestamp = timestamp,
    )
    add_data(db_data)

# CHECK DB
# print(get_all_configs())