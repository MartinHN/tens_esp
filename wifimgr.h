#include <FS.h>                   //this needs to be first, or it all crashes and burns...

#include <ESP8266WiFi.h>

//web config portal
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>

//Configuration saving
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson




//ticker flag for saving data
bool flag_SaveConfig = false;

#define TRIGGER_PIN 12 //start onDemand config portal when set to LOW
Ticker tkConfig ;
bool flag_ConfigPortal = false;
int timeout = 15000;

char ESP_NAME[40];
char UDP_PORT[10] = "12345";
char UDP_IP[16] = "239.0.0.57";



//callback notifying the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  flag_SaveConfig = true;
}

// Ticker flag to go to config mode
void CheckTriggerPin () {
  //Serial.println("Config check");
  if ( digitalRead(TRIGGER_PIN) == LOW) {
    flag_ConfigPortal = true;
  }
}

void ReadConfig() {
  
  //read configuration from FS json
  Serial.println("mounting FS...");
  delay(100);
  if (SPIFFS.begin()) {
  
  // UNCOMMENT TO FORMAT CONFIG FILE, UPLOAD, EXECUTE ONCE AND REUPLOAD WITH THIS COMMENTEND
//  Serial.println("Please wait 30 secs for SPIFFS to be formatted");
//  SPIFFS.format();
//  Serial.println("Spiffs formatted");
  
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          strcpy(ESP_NAME, json["ESP_NAME"]);
          strcpy(UDP_PORT, json["UDP_PORT"]);
          strcpy(UDP_IP, json["UDP_IP"]);
          Serial.print("ESP NAME: ");
          Serial.println(ESP_NAME);
          Serial.print("ESP PORT: ");
          Serial.println(UDP_PORT);
          Serial.print("ESP IP: ");
          Serial.println(UDP_IP);
          

        } 
        else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read
}

void WriteConfig() {

  //save the custom parameters to FS
    if (flag_SaveConfig) {
      
      flag_SaveConfig = false ;
      Serial.println("saving config");
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["ESP_NAME"] = ESP_NAME;
      json["UDP_PORT"] = UDP_PORT;
      json["UDP_IP"] = UDP_IP;

      File configFile = SPIFFS.open("/config.json", "w");
      if (!configFile) {
        Serial.println("failed to open config file for writing");
      }
  
      json.printTo(Serial);
      json.printTo(configFile);
      configFile.close();
      Serial.println();
      //end save
    }
}

void StartConfigAP(){  
  /* stops all tickers, start config portal and waits for new configuration
   *  if new connection, saves the new configuration values and goes back to loop()
   add a check if disconnected ?
   */

  if (flag_ConfigPortal) { //or disconnected from wifi
    
    flag_ConfigPortal = false; //reset flag
    
    // detach all tickers (redLed, blueLed, OTA, wifimgr, UDP)
    redLedState (1, 500);
    blueLedState (1,500);
    //detachOTA();
    tkConfig.detach();
    delay (500); 

    //ReadConfig() ; //read config.json from FS
    
    //WiFiManager
    
    // The extra parameters to be configured (can be either global or just in the setup)
    // After connecting, parameter.getValue() will get you the configured value
    // id/name placeholder/prompt default length
    WiFiManagerParameter custom_ESP_NAME("name", "ESP NAME", ESP_NAME, 40);
    WiFiManagerParameter custom_UDP_PORT("port", " UDP port", UDP_PORT, 10);
    WiFiManagerParameter custom_UDP_IP("ip", "UDP IP", UDP_IP, 32);
  
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    
    //wifiManager.resetSettings();//reset settings - for testing    
    wifiManager.setSaveConfigCallback(saveConfigCallback);//set config save notify callback

    //add all your parameters here
    wifiManager.addParameter(&custom_ESP_NAME);
    wifiManager.addParameter(&custom_UDP_PORT);
    wifiManager.addParameter(&custom_UDP_IP);

    //it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration

    
    delay(1000);

    //Modify below to restart config portal 
   // bool flag_connected =false ;
    //while (!flag_connected) {
      redLedState (1, 100);
      blueLedState (-1, 100);
      if (!wifiManager.startConfigPortal(ESP_NAME)) {
        Serial.println("failed to connect, restarting config portal");
        //delay(2000);
        //reset and try again
        redLedState (-1, 100);
        blueLedState (-1, 100);

        //Try to restart, but NOT using the wifi in the JSON but the 
        //ESP.reset();
        //delay(3000);
      }
     // else {flag_connected = true;}
    //}


    //if you get here you have connected to the WiFi
    Serial.println("connected to wifi");
    blueLedState(0,500);

    //read updated parameters
    strcpy(ESP_NAME, custom_ESP_NAME.getValue());
    strcpy(UDP_PORT, custom_UDP_PORT.getValue());
    strcpy(UDP_IP, custom_UDP_IP.getValue());

    WriteConfig();
    Serial.println("Restarting");
    delay(500);
    ESP.reset();
    delay(3000);
  }
}


void setupWifi() {
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  blueLedState (-1, 300);
  
  ReadConfig();
  Serial.print("Connecting");
  WiFi.mode(WIFI_STA);  // force remove AP
  // WIFI SEPCTACLE SSID sniperAP MDP razorshark
  WiFi.begin("Nonlieu","nonlieu67");
  //WiFi.begin();
  int start_time = millis();
  while(WiFi.status()!=3){
    Serial.print(".");
    delay(1000);
    if (millis()-start_time > timeout) {
      Serial.println("timeout, starting config portal");
      flag_ConfigPortal = true;
      StartConfigAP();
    }
  }
  Serial.println();
  Serial.print("connected to ");  //add wifi ssid 
  Serial.println(WiFi.SSID());
  blueLedState(1,500);
  //ajout attente connection + leds
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());// ou vérifier si IP = 0.0.0.0, lancer config portal
  tkConfig.attach(5, CheckTriggerPin); // check TRIGGER_PIN state periodically
  Serial.print("ESP_NAME: ");
  Serial.println(ESP_NAME);
  //delay(10000);
  
//  while(WiFi.status()<3) {
//    Serial.print
//clean FS, for testing
  //SPIFFS.format();
  //}
  
}
