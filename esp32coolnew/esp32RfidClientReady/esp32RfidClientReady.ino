#include <SoftwareSerial.h>
String stream;
SoftwareSerial master (2,3);

#include "SPI.h"
#include "MFRC522.h"
#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;


void setup() {
  Serial.begin(115200);
  master.begin(115200);
  SPI.begin();
  rfid.PCD_Init();
}

void loop() {
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
  Serial.println("okay");
  strID.toUpperCase();
  stream = "tag:"+strID+":";
  Serial.println(strID);
  master.println(stream);
  delay(1000);
 
  stream="";  
}
