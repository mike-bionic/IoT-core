from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from flask_login import LoginManager
from datetime import date,datetime,time
app = Flask (__name__)
app.config['SECRET_KEY'] = "nj92uf923fbb02ubfuvb492bfv2p42"
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///meterData.db'
db = SQLAlchemy(app)

### Devices models ###
class Devices(db.Model):
	id = db.Column(db.Integer,primary_key=True)
	device_name = db.Column(db.String(100),nullable=False)
	measurementId = db.Column(db.Integer,db.ForeignKey("measurements.id"))

class Measurements(db.Model):
	id = db.Column(db.Integer,primary_key=True)
	value = db.Column(db.String(100),nullable=False)
	date = db.Column(db.DateTime,nullable=False,default=datetime.utcnow)
	devices = db.relationship('Devices',backref='measurements',lazy=True)

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
db.session.commit()

admin = User(username="admin",password="admin123",full_name="Administrator")
db.session.add(admin)


db.session.commit()
