from flask import Flask,render_template,url_for,redirect,jsonify,request
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
	user = db.relationship('User',backref='devices',lazy=True)
	apiKey = db.Column(db.String(500))
	paid = db.Column(db.Boolean,nullable=False,default=False)
	fullAmount = db.Column(db.Integer)
	monthlyFreeWaterAmount = db.Column(db.Integer,nullable=False,default=500)
	monthlyTresholdAmount = db.Column(db.Integer,nullable=False,default=3000)

class Measurements(db.Model):
	id = db.Column(db.Integer,primary_key=True)
	value = db.Column(db.String(100),nullable=False)
	date = db.Column(db.DateTime,nullable=False,default=datetime.now())
	deviceId = db.Column(db.Integer,db.ForeignKey("devices.id"))

###  end of Devices models ###

@app.route("/measurement/<key>/<int:val>")
def measurement(key,val):
	try:
		device = Devices.query.filter_by(apiKey=key).first()
		measurement = Measurements(value=val,deviceId=device.id)
		db.session.add(measurement)
		device.fullAmount += measurement.value
		db.session.commit()

		if(device.fullAmount>=device.monthlyTresholdAmount):
			permission = "deny"
		else:
			permission = "accept"
		return jsonify({
			"response":"OK",
			"permission":permission
			})
	except:
		return jsonify({"response":"error"})

@app.route("/")
@app.route("/chart")
@app.route("/main")
def home():
	device = Devices.query.get(1)
	dailyMeasured = []
	measurements = Measurements.query.filter_by(deviceId=device.id).all()
	
# there's an error here, will check it later

	# for measurement in measurements:
	# 	print(measurement.date)
	# 	if len(dailyMeasured)==0:
	# 		dailyMeasured.append(measurement)
	# 	else:
	# 		for days in dailyMeasured:
	# 			if (days.date.strftime("%d") != measurement.date.strftime("%d")):
	# 				dailyMeasured.append(measurement)
	# 				print(measurement.value)
	# 				print("appended")
	# 			elif (days.date.strftime("%d") == measurement.date.strftime("%d")):
	# 				print(days.value)
	# 				print(measurement.value)
	# 				days.value=int(days.value)+int(measurement.value)
	# 				print("value Up")

	# print("_________________-")
	# print(dailyMeasured)
	# for days in dailyMeasured:
	# 	print(days.value)
####################

	commonUsedAmount=0
	pastMeasurement=0
	monthlyFreeWaterAmount = device.monthlyFreeWaterAmount
	monthlyTresholdAmount = device.monthlyTresholdAmount
	for measurement in measurements:
		commonUsedAmount += int(measurement.value)-pastMeasurement
		# pastMeasurement = int(measurement.value)


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



@app.route("/settings",methods=['GET','POST'])
# @login_required
def settings():
	user = User.query.get(1)
	device = Devices.query.get(1)
	if request.method == 'POST':
		monthlyFreeWaterAmount=request.form.get("monthlyFreeWaterAmount")
		monthlyTresholdAmount=request.form.get("monthlyTresholdAmount")
		try:
			device.monthlyFreeWaterAmount=monthlyFreeWaterAmount
			device.monthlyTresholdAmount=monthlyTresholdAmount
			db.session.commit()
		except:
			print('error')
	return render_template("settings.html",user=user,device=device)

@app.route("/card_register",methods=['POST'])
def card_register():
	user = User.query.get(1)
	personalTag = request.form.get('personalTag')
	print(personalTag)
	user.personalTag = personalTag
	user.lastTagRegTime = datetime.now()
	db.session.commit()
	return redirect("/")


@app.route("/card/pay/<cardId>")
def card_pay(cardId):
	try:
		user = User.query.filter_by(personalTag=cardId).first()
		print(user.username)
		device = Devices.query.filter_by(id=user.deviceId).first()
		device.fullAmount = 0
		user.lastTagRegTime=(datetime.now())
		# measurement = Measurements(value=0,deviceId=device.id,paid=True)
		# db.session.add(measurement)
		db.session.commit()
		if(device.fullAmount>=device.monthlyTresholdAmount):
			permission = "deny"
		else:
			permission = "accept"
		return jsonify({
			"response":"OK",
			"permission":permission
			})
	except:
		return jsonify({
			"response":"Wrong card Id"
			})

command = {"command":''}
@app.route("/command/<pin>/<state>")
def command(pin,state):
	global command
	command = {"command":str(pin)+":"+str(state)+":"}
	return redirect("/control")

@app.route("/checkCommand")
def checkCommand():
	global command
	return command

@app.route("/tables")
def tables():
	user = User.query.get(1)
	device = Devices.query.get(1)
	measurements = Measurements.query.filter_by(deviceId=device.id).all()
	# measurements = Measurements.query.filter_by(deviceId=device.id)\
	# .filter(date>=user.lastTagRegTime).all()
	
	return render_template("tables.html",measurements=measurements,
		device=device)

@app.route("/control")
def control():
	return render_template("control.html")

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
	personalTag = db.Column(db.String(120))
	lastTagRegTime = db.Column(db.DateTime)
	deviceId = db.Column(db.Integer,db.ForeignKey("devices.id")) 
	def __repr__ (self):
		return f"User('{self.username}')"

if __name__ == "__main__":
	app.run(host="0.0.0.0" , port=5100 , debug=True)