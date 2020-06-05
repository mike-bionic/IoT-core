#include <SoftwareSerial.h>
String stream;
SoftwareSerial master (2,3);

#include "SPI.h"
#include "MFRC522.h"
#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;


///////////////////////////////
volatile int flow_frequency; // Measures flow sensor pulses
unsigned int l_hour; // Calculated litres/hour
unsigned char flowsensor = 2; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;
void flow () // Interrupt function
{
   flow_frequency++;
}
//////////////////////////////////

void setup() {
  Serial.begin(115200);
  master.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH);
  attachInterrupt(0, flow, RISING); // Setup Interrupt
  sei(); // Enable interrupts
  currentTime = millis();
  cloopTime = currentTime;

}

void loop() {

  currentTime = millis();
  // Every second, calculate and print litres/hour
  if(currentTime >= (cloopTime + 10000))
  {
    cloopTime = currentTime; // Updates cloopTime
    // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
    l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
    flow_frequency = 0; // Reset Counter
    Serial.print(l_hour, DEC); // Print litres/hour
    Serial.println(" L/hour");
    long value = millis();
    stream = "waterSensor:"+String(value)+":";
    Serial.println(stream);
    master.println(stream);
    stream = "";
  }
  
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    return;
  }
  String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
      (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
      String(rfid.uid.uidByte[i], HEX) +
      (i != 3 ? " " : "");
  }

  strID.toUpperCase();
  stream = "tag:"+strID+":";
  Serial.println(stream);
  master.println(stream);
  delay(1000);
 
  stream="";

}