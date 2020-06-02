from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from flask_login import LoginManager
from datetime import date,datetime,time
# import datetime

app = Flask (__name__)
app.config['SECRET_KEY'] = "nj92uf923fbb02ubfuvb492bfv2p42"
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///meterData.db'
db = SQLAlchemy(app)

### Devices models ###
class Devices(db.Model):
	id = db.Column(db.Integer,primary_key=True)
	device_name = db.Column(db.String(100),nullable=False)
	measurements = db.relationship('Measurements',backref='devices',lazy=True)
	apiKey = db.Column(db.String(500))

class Measurements(db.Model):
	id = db.Column(db.Integer,primary_key=True)
	value = db.Column(db.String(100),nullable=False)
	date = db.Column(db.DateTime,nullable=False,default=datetime.now())
	deviceId = db.Column(db.Integer,db.ForeignKey("devices.id"))

###  end of Devices models ###

# #### auth and login routes ####
from flask_login import UserMixin

class User(db.Model, UserMixin):
	id = db.Column(db.Integer,primary_key=True)
	username = db.Column(db.String(50),unique=True,nullable =False)
	full_name = db.Column(db.String(100))
	password = db.Column(db.String(100), nullable=False)
	def __repr__ (self):
		return f"User('{self.username}')"


db.drop_all()
db.create_all()

device = Devices(device_name="waterMeter",apiKey="fw3445g46423527hef2")
db.session.add(device)

admin = User(username="admin",password="admin123",full_name="Administrator")
db.session.add(admin)

measurement = Measurements(value=122,date=datetime(2020,6,2,2,0,0,0),deviceId=1)
db.session.add(measurement)
measurement = Measurements(value=30,date=datetime(2020,6,4,4,0,0,0),deviceId=1)
db.session.add(measurement)
measurement = Measurements(value=23,date=datetime(2020,6,5,8,0,0,0),deviceId=1)
db.session.add(measurement)
measurement = Measurements(value=87,date=datetime(2020,6,7,9,0,0,0),deviceId=1)
db.session.add(measurement)
measurement = Measurements(value=134,date=datetime(2020,6,8,10,0,0,0),deviceId=1)
db.session.add(measurement)
measurement = Measurements(value=55,date=datetime(2020,6,10,12,0,0,0),deviceId=1)
db.session.add(measurement)
measurement = Measurements(value=43,date=datetime(2020,6,14,23,0,0,0),deviceId=1)
db.session.add(measurement)
measurement = Measurements(value=338,date=datetime(2020,6,15,2,0,0,0),deviceId=1)
db.session.add(measurement)


# # if measurement always rises
# measurement = Measurements(value=10,date=datetime(2020,6,2,2,0,0,0),deviceId=1)
# db.session.add(measurement)
# measurement = Measurements(value=30,date=datetime(2020,6,4,4,0,0,0),deviceId=1)
# db.session.add(measurement)
# measurement = Measurements(value=45,date=datetime(2020,6,5,8,0,0,0),deviceId=1)
# db.session.add(measurement)
# measurement = Measurements(value=87,date=datetime(2020,6,7,9,0,0,0),deviceId=1)
# db.session.add(measurement)
# measurement = Measurements(value=99,date=datetime(2020,6,8,10,0,0,0),deviceId=1)
# db.session.add(measurement)
# measurement = Measurements(value=142,date=datetime(2020,6,10,12,0,0,0),deviceId=1)
# db.session.add(measurement)
# measurement = Measurements(value=456,date=datetime(2020,6,14,23,0,0,0),deviceId=1)
# db.session.add(measurement)
# measurement = Measurements(value=498,date=datetime(2020,6,15,2,0,0,0),deviceId=1)
# db.session.add(measurement)


db.session.commit()
