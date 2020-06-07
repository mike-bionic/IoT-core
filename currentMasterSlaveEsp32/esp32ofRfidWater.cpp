#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
const char* ssid = "cyber";
const char* password = "nanoelectronics";

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

String serverUrl = "192.168.43.69:5100";
String apiKey = "fw3445g46423527hef2";

int value;

String payload;

String stream;
String action;
String val;

int stove1 = 34;
int stove2 = 35;
int extractorFan = 32;
int waterPin = 33;
int pin25 = 25;
int pin26 = 26;
int pin27 = 27;
int pin14 = 14;
int pin12 = 12;
int pin13 = 13;

void setup() {

  pinMode(stove1,OUTPUT);
  pinMode(stove2,OUTPUT);
  pinMode(extractorFan,OUTPUT);
  pinMode(waterPin,OUTPUT);
  pinMode(pin25,OUTPUT);
  pinMode(pin26,OUTPUT);
  pinMode(pin27,OUTPUT);
  pinMode(pin14,OUTPUT);
  pinMode(pin12,OUTPUT);
  pinMode(pin13,OUTPUT);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  if (Serial.available()){
    stream = Serial.readStringUntil('\n');
    stream.trim();
    if (stream.length()>0){
      action = getStringPartByNr(stream,':',0);
      val = getStringPartByNr(stream,':',1);

      if(action=="tag"){
        Serial.println("Tag Received: ");
        Serial.println(val);
        val.replace(" ", "%20");

        sendRequest("http://"+serverUrl+"/card/pay/",val);
      }
      if(action=="waterSensor"){
        Serial.println("Tag Received: ");
        Serial.println(val);

        sendRequest("http://"+serverUrl+"/measurement/"+apiKey+"/",val);
      }
    }
    Serial.println(stream);
    stream="";
  }
}


String httpGETRequest(const char* serverName) {
  HTTPClient http;
  http.begin(serverName);
  int httpResponseCode = http.GET();  
  String payload = "{}"; 
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  return payload;
}

//////// splitting text by delimeter : /////
String getStringPartByNr(String data, char separator, int index){
  int stringData = 0;
  String dataPart = "";
  for(int i = 0; i<data.length()-1; i++){
    if(data[i]==separator) {
      stringData++;
    }else if(stringData==index) {
      dataPart.concat(data[i]);
    }else if(stringData>index){
      return dataPart;
      break;
    }
  }
  return dataPart;
}

void sendRequest(String path, String sendingData){
  if(WiFi.status()== WL_CONNECTED){
    String serverPath = path+sendingData;
    Serial.println(serverPath);
    payload = httpGETRequest(serverPath.c_str());
    Serial.println(payload);

    const size_t capacity = JSON_OBJECT_SIZE(3)+JSON_ARRAY_SIZE(2)+60;
    DynamicJsonBuffer jsonBuffer(capacity);

    JsonObject& root = jsonBuffer.parseObject(payload);

    String response = root["response"].as<String>();
    String permission = root["permission"].as<String>();
    Serial.println("permission:"+permission+":");
    Serial.println(response);

    if(response=="got the card"){
      Serial.println("It's working, OKAY");
    }
    String command = root["command"].as<String>();
    if(command>0){
      action = getStringPartByNr(command,':',0);
      val = getStringPartByNr(command,':',1);
      digitalWrite(action,toInt(val));
    }
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}