from flask import Flask,render_template,url_for,flash,redirect,request,Response,abort
from flask_login import login_user, current_user, logout_user, login_required
from flask_sqlalchemy import SQLAlchemy
from flask_login import LoginManager

from datetime import date,datetime,time
app = Flask (__name__)

app.config['SECRET_KEY'] = "nj92uf923fbb02ubfuvb492bfv2p42"
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///meterData.db'

db = SQLAlchemy(app)
login_manager = LoginManager(app)

login_manager.login_view = 'main'
login_manager.login_message = 'Ulgama giriň!'
login_manager.login_message_category = 'info'


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


#### interactive pages of view ####
@app.route("/devices/<int:id>",methods=['GET','POST'])
def devices_manage(id):
	print ('Got the request')
	return (id)

@app.route("/")
def devices():
	print("We're connected")
	return ("ok!")

#############################


# #### auth and login routes ####
from flask_login import UserMixin

@login_manager.user_loader
def load_user(id):
	return User.query.get(int(id))

class User(db.Model, UserMixin):
	id = db.Column(db.Integer,primary_key=True)
	username = db.Column(db.String(50),unique=True,nullable =False)
	full_name = db.Column(db.String(100))
	password = db.Column(db.String(100), nullable=False)
	def __repr__ (self):
		return f"User('{self.username}')"


if __name__ == "__main__":
	app.run(host="0.0.0.0" , port=5000 , debug=True)