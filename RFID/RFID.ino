#include<SPI.h>
#include<MFRC522.h>
MFRC522 mfrc522(10,9);

#define BUZZER 7
#define RED 6
#define GREEN 5

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, HIGH);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);
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

void statusOK()
{
  digitalWrite(BUZZER, LOW);
  digitalWrite(GREEN, HIGH);
  delay(100);
  digitalWrite(BUZZER, HIGH);
  digitalWrite(GREEN, LOW);
  delay(100);
}

void statusBAD()
{
  for(int i = 0; i < 3; i++)
  {
    digitalWrite(BUZZER, LOW);
    digitalWrite(RED, HIGH);
    delay(50);
    digitalWrite(BUZZER, HIGH);
    digitalWrite(RED, LOW);
    delay(50);
  }
}

byte recv = 0;
void loop() {
  recv = Serial.read();
  if(recv == '1') statusOK();
  else if(recv == '2') statusBAD();
  recv = 0;
  
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
