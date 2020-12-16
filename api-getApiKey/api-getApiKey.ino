
#ifdef ESP8266
#include <ESP8266WiFi.h>    
#include "ESP8266HTTPClient.h"
#else
#include <WiFi.h>     
#include <HTTPClient.h>
#endif
const char* wifissid="WIFIssid";
const char* wifipassword="WIFIpassword";
String userid="CavyIoT_username";
String password="CavyIoT_passwod";
String rbody;
HTTPClient http;
void EspPostMethod(String rbody)
 {
  http.begin("http://api.cavyiot.com/device/get-apikey"); 
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(rbody);
  if(httpCode<0){
    http.end();
     Serial.println("...Server busy.. Trying to attempt again please wait.. ");
     EspPostMethod(rbody);
    }
  else{
     Serial.println("");
     Serial.println(http.getString());
     http.end();
    }
  }
void setup() {
 Serial.begin(9600);        
 WiFi.begin(wifissid,wifipassword); 
 while (WiFi.status() != WL_CONNECTED ) 
          { 
          delay(500); 
           Serial.print(".");
          }
   rbody="{\"userid\":\""+userid+"\",\"password\":\""+password+"\"}";             
   EspPostMethod(rbody); 
    
}

void loop() {
  // put your main code here, to run repeatedly:

}
