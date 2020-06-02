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
	measurements = db.relationship('Measurements',backref='devices',lazy=True)
	apiKey = db.Column(db.String(500))

class Measurements(db.Model):
	id = db.Column(db.Integer,primary_key=True)
	value = db.Column(db.String(100),nullable=False)
	date = db.Column(db.DateTime,nullable=False,default=datetime.now())
	deviceId = db.Column(db.Integer,db.ForeignKey("devices.id"))


###  end of Devices models ###

#### interactive pages of view ####
@app.route("/devices/<int:id>",methods=['GET','POST'])
def devices_manage(id):
	print ('Got the request')
	return (id)


monthlyFreeWaterAmount = 950
monthlyTresholdAmount = 4000

@app.route("/")
@app.route("/home")
@app.route("/main")
def home():
	device = Devices.query.get(1)
	measurements = Measurements.query.filter_by(deviceId=device.id)

	commonUsedAmount=0
	pastMeasurement=0
	for measurement in measurements:
		commonUsedAmount += int(measurement.value)-pastMeasurement
		# pastMeasurement = int(measurement.value)

	global monthlyFreeWaterAmount
	global monthlyTresholdAmount

	# calculates current amount of free if not expired

	if commonUsedAmount<monthlyFreeWaterAmount:
		usedFreeWaterPercentage = (100*commonUsedAmount)/monthlyFreeWaterAmount
		nonFreeWaterPercentage = 0
	else:
		usedFreeWaterPercentage=100

		nonFreeWaterPercentage = (100*(commonUsedAmount-monthlyFreeWaterAmount))/monthlyTresholdAmount

	if commonUsedAmount<monthlyTresholdAmount:
		usedCommonWaterPercentage = (100*commonUsedAmount)/monthlyTresholdAmount
	else:
		usedCommonWaterPercentage = 100

	return render_template("main.html",
		device=device, measurements=measurements,
		monthlyFreeWaterAmount=monthlyFreeWaterAmount,
		monthlyTresholdAmount=monthlyTresholdAmount,
		commonUsedAmount=commonUsedAmount,
		usedCommonWaterPercentage=usedCommonWaterPercentage,
		usedFreeWaterPercentage=usedFreeWaterPercentage,
		nonFreeWaterPercentage=nonFreeWaterPercentage)

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
	app.run(host="0.0.0.0" , port=5100 , debug=True)