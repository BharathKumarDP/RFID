#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include<Wire.h>

#define RST_PIN D3
#define SS_PIN D4
#define LED_G D8 
#define LED_R D1
#define BUZZER D0



String MasterTag = "b9eae8a3";  
String tagID = "";

// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
//LiquidCrystal lcd(14, 10, 9, 13, 11, 12); //Parameters: (rs, enable, d4, d5, d6, d7) 
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() 
{
  // Initiating
  pinMode(LED_R,OUTPUT);
  pinMode(LED_G,OUTPUT);
  pinMode(BUZZER,OUTPUT);
  Serial.begin(9600);
  SPI.begin(); // SPI bus
  mfrc522.PCD_Init(); // MFRC522
  lcd.init();
  lcd.clear();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print(" Access Control ");
    lcd.setCursor(0, 1);
    lcd.print("Scan Your Card");
    delay(1000);
  
}
void Siren()
{
  for (int hz = 440; hz < 1000; hz++) {
    tone(BUZZER, hz, 50);
    delay(5);
  }

  for (int hz = 1000; hz > 440; hz--) {
    tone(BUZZER, hz, 50);
    delay(5);
  }
  digitalWrite(BUZZER, LOW);
}

void loop() 
{
  
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
  
  
  while (getID()) 
  {
    
    
    if (tagID == MasterTag) 
    {
      
      lcd.print(" Access Granted!");
      digitalWrite(LED_G,HIGH);
      tone(BUZZER,1000,250);
      delay(1000);
      digitalWrite(LED_G,LOW);
      tone(BUZZER,6000,250);
       
      
    }
    else
    {
      lcd.print(" Access Denied!");
      Siren();
      digitalWrite(LED_R,HIGH);
      delay(1000);
      digitalWrite(LED_R,LOW);
      
     }
    
      lcd.setCursor(0, 1);
      lcd.print(" ID : ");
      lcd.print(tagID);
      tagID="";
      delay(1000);
     
   }
   
  
}


boolean getID() 
{ 
  
  if ( ! mfrc522.PICC_IsNewCardPresent())
    return false;
  if (mfrc522.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tagID += String(mfrc522.uid.uidByte[i],HEX);
    }
    Serial.println(tagID);
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return true;
  }
  return false;
  
}
