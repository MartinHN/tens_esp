//OSC via udp
#include <WiFiUdp.h>
#include <OSCMessage.h>

WiFiUDP UDP; //udp listener

/* UDP CONFIGURATION */
//int UDP_In_Port = 9000;  //udp port input for ESP
//
////default address and port to send to (IP read from config)
//IPAddress UDP_Out_IP ;
//int UDP_Out_Port = 8000 ;
IPAddress ipMulti(239, 0, 0, 56);
//IPAddress ipMulti(192, 168, 1, 255);
unsigned int portMulti = 12345;      // local port to listen on

template <typename TYPE> void sendOSC(const char * adress, TYPE parameter);


template <typename TYPE> void sendOSC(const char * adress, TYPE parameter) {
  OSCMessage OSCmsg(adress);
  OSCmsg.add(parameter);
  UDP.beginPacketMulticast(ipMulti, portMulti,WiFi.localIP());
  OSCmsg.send(UDP); // send the bytes to the SLIP stream
  UDP.endPacket(); // mark the end of the OSC Packet
  OSCmsg.empty(); // free space occupied by message
}

void printTest(OSCMessage &msg) {
  
    Serial.print("OSC type : ");
    Serial.println (msg.getType(0));
    sendOSC("/received", (int) msg.getType(0));
    
  
}

void updatePotValue(OSCMessage &msg) {
  // message /ESP_NAME/pot pot#[0..3] value[0..255]
  potValueUpdate[msg.getInt(0)] = msg.getInt(1);
  sendOSC("/updated", "pot" ); 
  //Serial.print("update pot : " + msg.getInt(0) + "  ");
  Serial.println (potValueUpdate[msg.getInt(0)]);
}

void setup_OSC(){
  
  ipMulti.fromString(UDP_IP);
  
  String UDP_PORT_string = UDP_PORT;
  portMulti = UDP_PORT_string.toInt();

  Serial.print("connecting udp to ");
  Serial.print(ipMulti);
  Serial.print(":");
  Serial.println(portMulti);
  Serial.print("ESP NAME:");
  Serial.println(ESP_NAME);
  
  UDP.beginMulticast(WiFi.localIP(),  ipMulti, portMulti);
//  OSCMessage OSCmsg("/connected");
//  for (int i=0 ; i<4 ; i++){
//    OSCmsg.add(WiFi.localIP()[i]);
//  }
//  UDP.beginPacketMulticast(ipMulti, portMulti,WiFi.localIP());
//  OSCmsg.send(UDP); // send the bytes to the SLIP stream
//  UDP.endPacket(); // mark the end of the OSC Packet
//  OSCmsg.empty(); // free space occupied by message
  sendOSC("/connected", ESP_NAME);
}

void handle_OSC() {
  OSCMessage OSCin;
  int size;

  if ( (size = UDP.parsePacket()) > 0) {
    while (size--)
    OSCin.fill(UDP.read());

    //Declare valid OSC messages here 
    if (!OSCin.hasError()) {
      OSCin.dispatch("/test", printTest);
      if (OSCin.match(ESP_NAME)){
        Serial.println("matched");
        OSCin.dispatch("/ESP_TENS_*/pot", updatePotValue);
      
      
    }
  }
}
}
