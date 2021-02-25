from flask import request, abort, jsonify
from flask_login import login_required, current_user
from app.api import bp
from app.models import Device
from app import db
from cerberus import Validator
from flask import abort
from app.api.auth import token_auth, basic_auth

#Note token_auth is disabled as the backend never seems to recieve the token
#This may be because of our odd problems working with local host
#Change login_required to token_auth.login_required when we get a server

device_schema = {
                    "appliance_name": {"type": "string", "maxlength": 64, "nullable": True}, 
                    "device_state": {"type": "integer", "default": False, "nullable": False},
                    "device_battery": {"type": "float", "nullable": True}, # float change position reminder
                    "timestamp": {"type": "datetime", "nullable": False}
}

v = Validator(device_schema, allow_unknown=True)

#Multi use Route to get specific information about devices
@bp.route('/device/<id>', methods=['GET', 'PATCH', 'DELETE'])
@login_required
def device_get_patch_delete_by_id(id):
    #SELECT *
    #FROM device
    #WHERE device.id = id AND device.user_id = current_user id
    #We make sure that the current_user can't put down devices 
    #that they do not have access to
    myDevice = Device.query.filter_by(id=id, user_id=current_user.get_id()).first()
    
    #Returns the specific device
    if request.method == 'GET':
        returnValue = jsonify(myDevice.to_dict())
        db.session.close
        return returnValue, 200
    #Updates the device tuple with new information
    elif request.method == 'PATCH':
        if not v.validate(request.get_json()):
            abort(400, description=v.errors)
        myDevice.update(request.get_json())
        db.session.commit()
        returnValue = jsonify(myDevice.to_dict())
        db.session.close()
        return returnValue, 200
    #Deletes device from database
    elif request.method == 'DELETE':
        db.session.delete(myDevice)
        db.session.commit()
        db.session.close()
        return '', 204

#This function gets all of the devices that the user owns.
#login does not work correctly
@bp.route('/devices', methods=['GET'])
@login_required
def getUserDevices():
    results = Device.query.filter_by(user_id = current_user.get_id())
    myList = []
    for row in results:
        #TODO modify timestamp in row to something human readadble
        #TODO check if there's a way to convert time to local
        #TODO switch to 12 hour time
        #Format M/D/Y HR:MIN AM/PM
        myList.append(row.to_dict())
    db.session.close()

    return jsonify(myList), 200

    #Select * From Device
    #Where Device.user_id = user_id
   # deviceUserList = Device.query.filter_by(user_id=current_user.get_id()).all()
   #db.session.close
    #Converts the variable into a Python dictionary
    #Then it can be turned into a JSON for easier parsing.
   # return jsonify(deviceUserList)
    #return deviceUserList

#The get request for this route is never used.
@bp.route('/device', methods=['POST', 'GET'])
@login_required
def device_get_post():
    if request.method == 'POST':
        if not v.validate(request.get_json()):
            abort(400, description=v.errors)
        request.get_json().pop("id", None)
        new_device = Device(**request.get_json())
        db.session.add(new_device)
        db.session.commit()
        add_user_device(new_device.id)
        myobj = new_device.to_dict()
        db.session.close()
        return jsonify(myobj), 201
    elif request.method == 'GET':
        #Select *
        #From Device
        results = Device.query
        myList = []
        for row in results:
            myList.append(row.to_dict())
        db.session.close()
        return jsonify(myList), 200

#Adds a new user device to the database.
def add_user_device(device_id):
    user_id = current_user.get_id()
    new_user_device.user_id = user_id
    new_user_device.device_id = device_id
    db.session.add(new_user_device)
    db.session.commit()
    return jsonify(new_user_device.to_dict())