# import datetime
# import sys
# import asyncio
# from typing import Any, Optional, Generator, Tuple, Union
#
# from sqlalchemy import *
# from sqlalchemy import orm
# from sqlalchemy.ext.declarative import declarative_base
# from sqlalchemy.orm import *
#
# from crud import update_quantity_available_timer_discounts_db
# from database.models import *
# from crud.ke_account import *
# from utils import dt
#
# SQLALCHEMY_DATABASE_URL = f'postgresql+psycopg2://postgres:1dI0qAPGirMk@45.141.102.195:5432/postgres'
# engine = create_engine(SQLALCHEMY_DATABASE_URL, pool_size=100, max_overflow=0, connect_args={'connect_timeout': 300})
# Session = sessionmaker(autocommit=False, autoflush=False, bind=engine)
# Base: Any = declarative_base()
#
#
# def get_db() -> Generator[Session, None, None]:
#     db = None
#     try:
#         db = Session()
#         yield db
#     finally:
#         if db:
#             db.close()
#
#
# default_bin_step_percent = 4.0
#
# with next(get_db()) as db:
#     print(asyncio.run(get_ke_account_by_shop_id_db(7126, db)))


import matplotlib.pyplot as plt
from celluloid import Camera
import csv

camera = Camera(plt.figure())
with open('output.csv', 'r') as csvfile:
    reader = csv.reader(csvfile)
    next(reader)  # Skip header row
    for row in reader:
        row = row[1:-1]
        plt.scatter([float(val) for val in row[::2]],[float(val) for val in row[1::2]], color='black', s=3)
        camera.snap()
camera.animate().save('vis.gif')

# from celluloid import Camera
# from matplotlib import pyplot as plt
#
#
# f = open('output.csv', 'r')
#
# fig = plt.figure()
# camera = Camera(fig)
# for line in f.readlines()[1:]:
#     spline = line.split(',')[1:-1]
#     plt.scatter(list(map(float, spline[::2])), list(map(float, spline[1::2])), color='purple', s=3)
#     camera.snap()
# camera.animate().save('vis.gif')