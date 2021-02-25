from flask import request, abort, jsonify, Response, redirect
from flask_login import login_required, current_user, login_user
from cerberus import Validator
from app.models import User, Device
from app.api import bp
from app import db
from app.api.auth import token_auth

#Do we have to update user schema?
#Used with the validator to ensure that the incoming data is a user
user_schema = {
                    "username": {"type": "string", "maxlength": 64, "nullable": True}, 
                    "email": {"type": "string", "maxlength": 64, "nullable": True}
}

v = Validator(user_schema, allow_unknown=True)

#Multifunction route that does things depending on the user id
@bp.route('/user/<id>', methods=['GET', 'PATCH', 'DELETE'])
@login_required
def user_get_patch_delete_by_id(id):
    if current_user is None:
        db.session.close()
        abort(404, description="This user does not exist")
    #Returns the specific User
    if request.method == 'GET':
        #returnValue = jsonify(current_user.to_dict())
        myList = []
        test = User.query.filter_by(id = current_user.get_id())
        for row in test:
            myList.append(row.to_dict())
        db.session.close()
        return jsonify(myList), 200
    #Updates the user password
    elif request.method == 'PATCH':
        obj = request.get_json()
        if not v.validate(obj):
            abort(400, description=v.errors)
        # Note that this update function is specified in models.py
        if "password" in obj:
            current_user.set_password(obj['password'])
        current_user.update(obj) 
        db.session.commit()
        returnValue = jsonify(current_user.to_dict())
        db.session.close()
        return returnValue, 200
    #Removes the user and its devices from the database
    elif request.method == 'DELETE':
        user = User.query.filter_by(id = current_user.get_id())
        print(user)
        for o in user:
            db.session.delete(o)
            db.session.flush()
        db.session.delete(current_user)
        db.session.commit()
        db.session.close()
        return '', 204

#Logs the user in
@bp.route('/login', methods=['POST'])
def login():
    if request.method == 'POST':
        if not v.validate(request.get_json()):
            abort(400, description=v.errors)
        user_data = request.get_json()
        user_email = user_data['email']
        #SELECT *
        #FROM User
        #WHERE User.email = user_email
        check_user = User.query.filter_by(email=user_email).first()
        if not check_user or not check_user.check_password(user_data['password']):
            #error handler, if login is not successful
            abort(403, description="The credentials you entered were incorrect")
        result = login_user(check_user, force=True)
        db.session.close()
        if result:
            return '', 204
        else:
            return 'Unauthorized', 401
#Adds a new user
#TODO add username
@bp.route('/user', methods=['POST'])
def user_post():
    if request.method == 'POST':
        obj = request.get_json()
        print(obj)
        if not v.validate(obj):
            abort(400, description=v.errors)
        myPassword = obj.pop('password', None)
        new_user = User(**obj)
        new_user.set_password(myPassword)
        db.session.add(new_user)
        db.session.commit()
        returnValue = jsonify(new_user.to_dict())
        db.session.close()
        print("New user added")
        return returnValue, 201
