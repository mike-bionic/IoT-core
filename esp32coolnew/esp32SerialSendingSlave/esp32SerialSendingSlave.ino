
#include <SoftwareSerial.h>
String stream;
int led = 13;
SoftwareSerial master (2,3);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  master.begin(115200);
  pinMode(led,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()){
    stream = Serial.readStringUntil('\n');
    stream.trim();
    if (stream.length()>0){
      Serial.println(stream);
      master.println(stream);
      stream="";
    }
  }
}
