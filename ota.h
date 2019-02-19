#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>



Ticker tkOTA;  // periodic check if OTA available
int OTA_check_ms = 100;
bool flag_OTA = false;

void OTA_Flag(){
  flag_OTA = true ;
}

void checkOTA(){
    //change to ticker flag system 
    if (flag_OTA){
      flag_OTA = false ;
      ArduinoOTA.handle();
      //Serial.println("OTA check");
    }
    
}

void detachOTA(){
  tkOTA.detach();
}



void setupOTA(){
  ArduinoOTA.onStart([]() {
    Serial.println("Starting firmware update");
    redLedState(-1,100); // fast red blink
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("update done !");
    redLedState(0, 500);
    blueLedState(-1, 100); // 5 fast blue blink on end
    delay(500);
    blueLedState(0,100);
    //ledBlink(Blue_Led, 3, 100);

  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
    blueLedState(-1,500); //slow blue blink during progress

  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
    redLedState(-1, 250);
    blueLedState(-1,250);
  });
  ArduinoOTA.begin();
  Serial.println("OTA started");
  tkOTA.attach_ms(OTA_check_ms, OTA_Flag); 
}



