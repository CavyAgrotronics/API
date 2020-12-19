#ifdef ESP8266
#include <ESP8266WiFi.h>   
#include <ESP8266HTTPClient.h>
#else
#include <WiFi.h>     
#include <HTTPClient.h>
#endif
#include <ArduinoJson.h>
#include <DHT.h>
HTTPClient http;
#define DHTTYPE DHT11   
#define dht_dpin D3 //
DHT dht(dht_dpin, DHTTYPE);
unsigned long previousMillis = millis();
const char* wifissid="Yash/ayush";
const char* wifipassword="cavy123259";
String api_key= "72DAWHIx5VYmqS8yoLG";  //Replace this API-key with your own.
StaticJsonDocument<800> json_body;
float temperature,humidity;
void setup() {   
 Serial.begin(9600);        
 WiFi.begin(wifissid,wifipassword); 
 while (WiFi.status() != WL_CONNECTED ) 
          { 
          delay(500); 
           Serial.print(".");
          }
   }

void loop() {
  
  temperature=(dht.readTemperature());
  humidity=(dht.readHumidity());
   if( millis() > previousMillis +4000)
   {
    update_sensor_data(temperature,humidity);
    previousMillis=millis();
   }
}

void update_sensor_data(float temp,float hum)
 {    String rbody;
      JsonObject  sensor1 = json_body.createNestedObject();
                  sensor1["name"] = "Temperature";
                  sensor1["value"] = temp;
                  sensor1["unit"] = "C";
       JsonObject sensor2 = json_body.createNestedObject();
                  sensor2["name"] = "Humidity";
                  sensor2["value"] =  hum;
                  sensor2["unit"] =  "RH";
    serializeJson(json_body, rbody);
    json_body.clear();
    http.begin("http://api.cavyiot.com/"+api_key+"/sensor/update"); 
    int httpCode = http.POST(rbody);
     if(httpCode<0){
                http.end();
                Serial.println("...Server busy.. Trying to attempt again please wait.. ");
                }
    else  {
       Serial.println("");
       Serial.println(http.getString());
       http.end();
      }
  }
   
