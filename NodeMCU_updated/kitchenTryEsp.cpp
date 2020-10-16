#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "azat";
const char* password = "oguzhan85";
String serverUrl = "192.168.1.106:5100";
String apiKey = "fw3445g46423527hef2";

IPAddress staticIP(192, 168, 1, 245); //ESP static ip
IPAddress gateway(192, 168, 1, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns(8, 8, 8, 8);  //DNS

int value;

String payload;

String stream;
String action;
String val;

int stove1 = 5;
int stove2 = 4;
int extractorFan = 0;
int waterPin = 2;
int pin14 = 14;
int pin12 = 12;
int pin13 = 13;
int pin15 = 15;
int pin10 = 10;
int pin9 = 9;


ESP8266WebServer server(80);
 // sendRequest("http://"+serverUrl+"/card/pay/",val);

void handlePins() {
  String stream = server.arg("command");
  Serial.println("Command is: " + stream);
  stream.trim();
  if (stream.length()>0){
    action = getStringPartByNr(stream,':',0);
    val = getStringPartByNr(stream,':',1);
    // takes delimeters as pins and states
    Serial.println(val.toInt());
    if(val.toInt()!=0){
      digitalWrite(action.toInt(),1);
    }else{
      digitalWrite(action.toInt(),0);
    }

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
  server.send(200, "text/html", stream);
  stream="";
}


void setup() {
  pinMode(stove1,OUTPUT);
  pinMode(stove2,OUTPUT);
  pinMode(extractorFan,OUTPUT);
  pinMode(waterPin,OUTPUT);
  pinMode(pin14,OUTPUT);
  pinMode(pin12,OUTPUT);
  pinMode(pin13,OUTPUT);
  pinMode(pin15,OUTPUT);
  pinMode(pin10,OUTPUT);
  pinMode(pin9,OUTPUT);
  digitalWrite(stove1,0);
  digitalWrite(stove2,0);
  digitalWrite(extractorFan,0);
  digitalWrite(waterPin,0);
  digitalWrite(pin14,0);
  digitalWrite(pin12,0);
  digitalWrite(pin13,0);
  digitalWrite(pin15,0);
  digitalWrite(pin10,0);
  digitalWrite(pin9,0);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");
  WiFi.disconnect();
  WiFi.config(staticIP, subnet, gateway, dns);
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);

  // while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  // }
  Serial.println("");
  Serial.print("Connected! IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/control/", handlePins); 
  server.begin();
}

void loop() {
  server.handleClient();

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

void sendRequest(String path, String sendingData){
  if(WiFi.status()== WL_CONNECTED){
    String serverPath = path+sendingData;
    Serial.println(serverPath);
    payload = httpGETRequest(serverPath.c_str());
    Serial.println(payload);
    ///////////////
    const size_t capacity = JSON_OBJECT_SIZE(3)+JSON_ARRAY_SIZE(2)+60;
    DynamicJsonBuffer jsonBuffer(capacity);

    JsonObject& root = jsonBuffer.parseObject(payload);

    String response = root["response"].as<String>();
    String permission = root["permission"].as<String>();
    Serial.println("permission isssss: ");
    Serial.println(permission);
    Serial.println("-------");
    if(permission == "deny"){
      digitalWrite(waterPin,0);
    }
    else if(permission == "accept"){
      digitalWrite(waterPin,1);
    }

    if(response=="got the card"){
      Serial.println("It's working, OKAY");
    }
    String Command = root["command"].as<String>();
    if(Command>" "){
      action = getStringPartByNr(Command,':',0);
      val = getStringPartByNr(Command,':',1);
      int toState;
      
      if(val.toInt()!=0){
        digitalWrite(action.toInt(),1);
      }else{
        digitalWrite(action.toInt(),0);
      }
      
    }
  }
  else {
    Serial.println("WiFi Disconnected");
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