#include <Wire.h>

//#define AD_adress1 0x2c
//#define AD_adress2 0x2f
//#define AD_pot1 0x00
//#define AD_pot2 0x80

byte AD_adress1 = 0x2c;
byte AD_adress2 = 0x2f;
byte AD_pot1 = 0x00;
byte AD_pot2 = 0x80;

int potValue[4];
int potValueUpdate[4];
byte potAdress[4][2] = { {AD_adress1, AD_pot1},
                       {AD_adress1, AD_pot2},
                       {AD_adress2, AD_pot1},
                       {AD_adress2, AD_pot2}, };

void setup_AD5242() {
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.println("starting potentiometers");
//  Serial.println(AD_adress1); 
//  Serial.println(AD_pot1);
//  Serial.println(potAdress[0][0]);
//  for (int i ; i<4 ; i++) {
//    for (int j ; j<2 ; j++){
//      Serial.print (potAdress[i][j], HEX);
//      Serial.print ("-");
//    }
//  }
//Set potentiometers to 0
  for (int pot=0 ; pot<4 ; pot++){
      Wire.beginTransmission(potAdress[pot][0]);
      Wire.write(potAdress[pot][1]);
      Wire.write(potValue[pot]); 
      Wire.endTransmission();
    }
  
}
int debugSawCount = 0;

void handle_AD5242() {
  
  #if 1
  debugSawCount++;
  if(debugSawCount>255){debugSawCount =0;}
  potValueUpdate[0] = debugSawCount;
  delay(5000/255);
#endif
  for (int pot=0 ; pot<4 ; pot++){
     
//    Serial.println("pot");
//    Serial.print(potAdress[pot][0] + " : ");
//    Serial.println(potAdress[pot][1]);
    if (potValue[pot] != potValueUpdate[pot]) {
      potValue[pot] = potValueUpdate[pot];
      Wire.beginTransmission(potAdress[pot][0]);
      Wire.write(potAdress[pot][1]);
      Wire.write(potValue[pot]); 
      byte wireError = Wire.endTransmission();
      if(wireError){
      Serial.print("wire error : ");
      Serial.println(wireError);
      }
      else{
         Serial.println("wire success : ");
      }
    }
  }
}
