import os
from sqlalchemy import create_engine, MetaData, desc
from sqlalchemy.orm import sessionmaker
from db.model import Config
from dotenv import load_dotenv
load_dotenv()

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
            configs = session.query(Config).all()
        except:
            return {"error": ":("}
        finally:
            session.close()
            return configs

def add_config(config: Config):
    engine = start_connection()
    if not engine:
        return
    SessionFactory = sessionmaker(engine)
    with SessionFactory() as session:
        try:
            session.add(config)
            session.commit()
            print("Added config")
            session.close()
            return True
        except:
            session.close()
            return False

# CHECK DB
# print(get_all_configs())