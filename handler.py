import os
from flask import Flask, request
from config import Config
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime
import json

#These variables define the battery voltages
#We can use these to calculate the battery power level
#as a percentage.
Vmax = 4.2
Vmin = 3.0

#Creation of app and database
#TODO add security and error checking
app = Flask(__name__)
app.config.from_object(Config)
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
db = SQLAlchemy(app)

#from handler.api import bp as api_bp

@app.route('/')
def hello_world():
   return 'Do not access these pages using a web browser!'

#TODO add
@app.route('/addDevice', methods=['POST'])
def add_new_device():
   if request.method == "POST":
      new_device = Device(**request.get_json())
      db.session.add(new_device)
      db.session.commit()
      add_user_device(new_device.id)
      myobj = new_device.to_dict()
      db.session.close()
      return jsonify(myobj), 201

#We want to get the device iD, then modify the corresponding device
@app.route('/updateState/<int:device_id>', methods=['PATCH'])
def device_data_post(device_id):
   if request.method == "PATCH":
      from models import Device, BatteryLogger
      deviceStats = request.get_json()
      deviceStats = json.loads(deviceStats)
      #Set the time the state of the device changed to right now
      deviceStats['timestamp'] = datetime.now()
      

      #Convert voltage to battery power.
      deviceVoltage = deviceStats['device_battery']
      # (Vactual - Vmin) * 100
      # ______________________
      # (Vmax - Vmin)
      deviceVoltage = (deviceVoltage - Vmin) * 100
      deviceVoltage = deviceVoltage / (Vmax - Vmin)
      deviceVoltage = round(deviceVoltage, 1)
      deviceStats['device_battery'] = deviceVoltage

      #Select *
      #From Device
      #Where id = device_id
      #LIMIT 1
      myDevice = Device.query.filter_by(id=device_id).first()

      print(myDevice)
      myDevice.update(deviceStats)

      myStamp = BatteryLogger()
      #Temporary for adding device battery logs
      print(deviceStats)

      myStamp.device_battery = deviceStats['device_battery']
      myStamp.timestamp_time = deviceStats['timestamp']
      myStamp.device_id = device_id

      db.session.add(myStamp)
      db.session.commit()
      return "PATCH Successful"


if __name__ == '__main__':
   app.run(port = 5005)
