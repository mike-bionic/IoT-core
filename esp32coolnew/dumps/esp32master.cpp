#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
const char* ssid = "cyber";
const char* password = "nanoelectronics";

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

String serverUrl = "192.168.43.69:5100";
String apiKey = "fw3445g46423527hef2";

int value = 0;

String payload;

String stream;
String action;
String val;


// **** measurer setup **** //
volatile int flow_frequency; // Measures flow sensor pulses
unsigned int l_hour; // Calculated litres/hour
unsigned char flowsensor = 2; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;
void flow () // Interrupt function
{
   flow_frequency++;
}
// ************************ //


void setup() {
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




  // **** measurer setup **** //
  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH); // Optional Internal Pull-Up
  Serial.begin(9600);
  attachInterrupt(0, flow, RISING); // Setup Interrupt
  sei(); // Enable interrupts
  currentTime = millis();
  cloopTime = currentTime;
  // *********************** //

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
    }
    Serial.println(stream);
    stream="";
  }
  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status()== WL_CONNECTED){
      // measureWater();
      // String serverPath = "http://"+serverUrl+"/measurement/"+apiKey+"/"+value;
      // Serial.println(serverPath);
      // payload = httpGETRequest(serverPath.c_str());
      // Serial.println(payload);

      // const size_t capacity = JSON_OBJECT_SIZE(3)+JSON_ARRAY_SIZE(2)+60;
      // DynamicJsonBuffer jsonBuffer(capacity);

      // JsonObject& root = jsonBuffer.parseObject(payload);

      // String response = root["response"].as<String>();
      // Serial.println(response);
      // if (response == "OK"){
      //   Serial.println("success response hehehe");
      // }
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
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


// sendRequest("http://"+serverUrl+"/measurement/"+apiKey+"/",value)

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
    Serial.println(response);

    if(response=="got the card"){
      Serial.println("It's working, OKAY");
    }
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}


void measureWater(){
  currentTime = millis();
  // Every second, calculate and print litres/hour
  if(currentTime >= (cloopTime + 1000))
  {
    cloopTime = currentTime; // Updates cloopTime
    // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
    l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
    
    // flow_frequency = 0; // Reset Counter

    Serial.print(l_hour, DEC); // Print litres/hour
    Serial.println(" L/hour");

    // sending data
    // value = l_hour;
    value = millis();
    
    String serverPath = "http://"+serverUrl+"/measurement/"+apiKey+"/"+value;
    Serial.println(serverPath);
    payload = httpGETRequest(serverPath.c_str());
    Serial.println(payload);

    const size_t capacity = JSON_OBJECT_SIZE(3)+JSON_ARRAY_SIZE(2)+60;
    DynamicJsonBuffer jsonBuffer(capacity);

    JsonObject& root = jsonBuffer.parseObject(payload);

    String response = root["response"].as<String>();
    Serial.println(response);
    if (response == "OK"){
      Serial.println("success response hehehe");
      flow_frequency = 0;
      Serial.println("flow freq resetted");
    }
  }
}
