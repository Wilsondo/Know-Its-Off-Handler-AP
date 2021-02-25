from handler import db
from sqlalchemy import inspect, UniqueConstraint, TIMESTAMP
from sqlalchemy.orm import relationship
from werkzeug.security import generate_password_hash, check_password_hash
from flask_login import UserMixin
import base64
from datetime import datetime, timedelta
import os
##############
# This file defines each table in the database. For example, the Appliance table
# has several columns -- the name of the appliance, its ID, the status of the appliance,
# and its alert details.
##############
class BatteryLogger(db.Model):
    __name__ = "batterylogger"
    id = db.Column(db.Integer, primary_key=True, autoincrement=True)
    device_id = db.Column(db.Integer, nullable = False)
    timestamp_time = db.Column(db.TIMESTAMP, nullable = False)
    device_battery = db.Column(db.Float, nullable=True) # May change from float later
    #device = db.relationship('Device')

class Device(db.Model):
    __name__ = "device"
    id = db.Column(db.Integer, primary_key=True, autoincrement=True)
    appliance_name = db.Column(db.String(64), nullable=False)
    device_state = db.Column(db.Integer, default=False, nullable=False)
    device_battery = db.Column(db.Float, nullable=True) # May change from float later
    timestamp = db.Column(db.TIMESTAMP, nullable=True)
    user_id = db.Column(db.Integer, db.ForeignKey('user.id'))

    #To dictionary functions are used to format the data to make it easier to JSONIFY    
    def to_dict(self):
        return {c.key: getattr(self, c.key)
            for c in inspect(self).mapper.column_attrs}
            
    def update(self, myDict):
        for key, value in myDict.items():
            setattr(self, key, value)

    def __repr__(self):
        return '<Device {}>'.format(self.id)

class User(UserMixin, db.Model):
    __name__ = "user"
    id = db.Column(db.Integer, primary_key=True, autoincrement=True)
    password = db.Column(db.String(512), nullable=True)
    username = db.Column(db.String(64), nullable=True)
    email = db.Column(db.String(64), nullable=False)
    devices = db.relationship('Device', backref='owner', lazy='dynamic')

    #Adds a constraint to force emails to be unique
    UniqueConstraint('email', name='uix_1')

    token = db.Column(db.String(32), index=True, unique=True)
    token_expiration = db.Column(db.DateTime)

    def to_dict(self):
        return {c.key: getattr(self, c.key)
            for c in inspect(self).mapper.column_attrs}

    def update(self, myDict):
        for key, value in myDict.items():
            setattr(self, key, value)

    def set_password(self, password):
        self.password = generate_password_hash(password)

    def check_password(self, password):
        return check_password_hash(self.password, password)
    
    def get_token(self, expires_in=3600):
        now = datetime.utcnow()
        if self.token and self.token_expiration > now + timedelta(seconds=60):
            return self.token
        self.token = base64.b64encode(os.urandom(24)).decode('utf-8')
        self.token_expiration = now + timedelta(seconds=expires_in)
        db.session.add(self)
        return self.token

    def revoke_token(self):
        self.token_expiration = datetime.utcnow() - timedelta(seconds=1)

    @staticmethod
    def check_token(token):
        user = User.query.filter_by(token=token).first()
        if user is None or user.token_expiration < datetime.utcnow():
            return None
        return user
    
    # This is how the object looks when printed out.
    def __repr__(self):
        return '<User {}>'.format(self.username)
