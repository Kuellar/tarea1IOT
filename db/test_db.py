from sqlalchemy import create_engine, MetaData, desc
from sqlalchemy.orm import sessionmaker
from model import Config

try:
    db_host = 'localhost'
    db_user = 'root'
    db_password = '...'
    db_database = 'tarea_iot'
    DATABSE_URI=f'mysql+mysqlconnector://{db_user}:{db_password}@{db_host}/{db_database}'
    engine = create_engine(DATABSE_URI)
    connection = engine.connect()
    metadata = MetaData()
except Exception as e:
    print("ERROR DATABASE - ", e)

def get_all_configs():
    SessionFactory = sessionmaker(engine)
    with SessionFactory() as session:
        try:
            configs = session.query(Config).all()
        except:
            return {"error": ":("}
        finally:
            session.close()
            return configs

print(get_all_configs())