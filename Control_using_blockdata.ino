#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#define LED_G D8 
#define BUZZER D0
constexpr uint8_t RST_PIN = D3;    
constexpr uint8_t SS_PIN = D4;     
MFRC522 rfid(SS_PIN, RST_PIN); 
MFRC522::MIFARE_Key key;
String tag;
const char* msg="Hello";
byte readcard[4];
int block=2;
byte masterblock[16]={"Hello"};
byte readblock[18]; 
LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup() {
  
  Serial.begin(9600);
  SPI.begin(); 
  rfid.PCD_Init(); 
  pinMode(LED_G,OUTPUT);
  pinMode(BUZZER,OUTPUT);
  for(byte i=0;i<6;i++){
    key.keyByte[i]=0xFF;
  }
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Access Control ");
  lcd.setCursor(0, 1);
  lcd.print("Scan Your Card ");
  delay(1000);
 
}
void Siren()
{
  for (int hz = 440; hz < 1000; hz++) {
    tone(BUZZER, hz, 50);
    delay(5);
  }
  digitalWrite(BUZZER, LOW);
}

void loop() {
  
  lcd.backlight();
  lcd.clear();
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    
     readBlock(block, readblock);
   Serial.print("read block: ");
   for (int j=0 ; j<14 ; j++)
   {
     Serial.write (readblock[j]);
     
   }
   Serial.println("");
    
    
  
  
  if (isEqual(masterblock,readblock)) {
    lcd.print(" Access Granted!");
      Serial.println("Access Granted!");
      digitalWrite(LED_G, HIGH);
      tone(BUZZER,1000,250);
      delay(1000);
      digitalWrite(LED_G, LOW);
      tone(BUZZER,1000,250);
      
      
    } else {
      lcd.print(" Access Denied!");
      Serial.println("Access Denied!");
      Siren();
      
    }
   
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    lcd.setCursor(0, 1);
      lcd.print(" ID : ");      
      for (int j=0 ; j<5 ; j++)
   {
     lcd.print ((char)readblock[j]);
     
   }
      
       tag=""; 
    delay(1000);
  }
      
}


int readBlock(int blockNumber, byte arrayAddress[]) 
{
  int largestModulo4Number=blockNumber/4*4;
  int trailerBlock=largestModulo4Number+3;//determine trailer block for the sector

  //authentication of the desired block for access
  byte status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(rfid.uid));

  if (status != MFRC522::STATUS_OK) {
         Serial.print("PCD_Authenticate() failed (read): ");
         Serial.println(rfid.GetStatusCodeName((MFRC522::StatusCode)status));
         return 3;
  }


byte buffersize = 18;
status = rfid.MIFARE_Read(blockNumber, arrayAddress, &buffersize);
  if (status != MFRC522::STATUS_OK) {
          Serial.print("MIFARE_read() failed: ");
          Serial.println(rfid.GetStatusCodeName((MFRC522::StatusCode)status));
          return 4;
  }
  Serial.println("block was read");
}
bool isEqual(byte masterblock[],byte readblock[]){
  for(int i=0;i<16;i++){
    if(masterblock[i]!=readblock[i]){
      return false;
    }
  }
  return true;
} 
