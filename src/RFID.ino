#include<SPI.h>
#include<MFRC522.h>
MFRC522 mfrc522(10,9);

void setup() {
  Serial.begin(9600);
  //while(!Serial);
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);
  //mfrc522.PCD_DumpVersionToSerial();
}

char toHex(byte v)
{
  if(v < 10) return v + '0';
  else
  {
    switch(v)
    {
      case 10: return 'A';
      case 11: return 'B';
      case 12: return 'C';
      case 13: return 'D';
      case 14: return 'E';
      case 15: return 'F';
    }
  }
}

void loop() {
  if(!mfrc522.PICC_IsNewCardPresent()) return;
  if(!mfrc522.PICC_ReadCardSerial()) return;

  char uid[100];
  int c = 0;
  for(byte i = 0; i < mfrc522.uid.size; i++) {
    uid[c++] = toHex((mfrc522.uid.uidByte[i] >> 4) & 0xF);
    uid[c++] = toHex((mfrc522.uid.uidByte[i] >> 0) & 0xF);
  }
  uid[c++] = '\0';
  Serial.println(uid);
}
