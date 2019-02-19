/*
    TENS_ESP

 *remove UKI references (names etc)
 *build OSC commmunication style (cf M_Lavenne)
 *use multicast (see udp_multicast2)
 *create digipot control functions
 *create ID message filtering system using "ESP_NAME"
 *add multicast configuration option (0 or 1)

*/

#include <Ticker.h>
#include "AD5242.h" //
#include "leds.h"     //config and functions relative to leds
//#include "ota.h"      //config and functions relative to ota firmware updates
#include "wifimgr.h"   //config and functions relative to wifi and access point configuration and configuration permanent saving
#include "osc.h"  //some helpers functions



void setup ( ) {
  delay(500) ;
  Serial.begin(115200);
  Serial.println("Starting ESP8266");
  
  setupLeds();
  setup_AD5242() ;
  setupWifi(); 
  setup_OSC() ;
  //setupOTA();
  
 
  blueLedState(-1, 500);
  
}


void loop () {
  //Serial.println(WiFi.status());
  StartConfigAP();
  //checkOTA();
  handle_OSC();
  handle_AD5242();
  
}




