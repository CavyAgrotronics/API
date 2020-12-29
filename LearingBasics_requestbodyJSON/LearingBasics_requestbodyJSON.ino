#ifdef ESP8266
#include <ESP8266WiFi.h>   
#include <ESP8266HTTPClient.h>
#define BUTTON_0 D2
#define BUTTON_1 D5
#define BUTTON_2 D6
#define BUTTON_3 D7
#elif ESP32
#define BUTTON_0 15
#define BUTTON_1 16
#define BUTTON_2 17
#define BUTTON_3 18
#include <WiFi.h>     
#include <HTTPClient.h>
#endif
#include <ArduinoJson.h>
#include<DHT.h>
#define DHTTYPE DHT11   
#define dht_dpin D3 //
DHT dht(dht_dpin, DHTTYPE);

const char* wifissid="WiFiSSID";
const char* wifipassword="WiFiPassword";
String payload;
StaticJsonDocument<1200> json_body;
String api_key= "xTBcn1HZo9Yd6fCpOAKm";  //Replace this API-key with your own.
/*
       If you dont have account on cavyiot.com then register now and
after veriication of your email you can add upto five devices free.
Your device APIkey you can see in you client area after your next login.
       In client area you can add devices, sensors and can set trigger
logic for your devices to control automatically and after select from 
upper top menu 'control panel' you can access your device via this remote 
control panel.
*/
float temperature;
float humidity;
String rbody;
HTTPClient http;

void setup() {
  pinMode(BUTTON_0 , OUTPUT);digitalWrite(BUTTON_0, HIGH);
  pinMode(BUTTON_1 , OUTPUT);digitalWrite(BUTTON_1, HIGH);
  pinMode(BUTTON_2 , OUTPUT);digitalWrite(BUTTON_2, HIGH);
  pinMode(BUTTON_3 , OUTPUT);digitalWrite(BUTTON_3, HIGH);
 Serial.begin(9600);        
 WiFi.begin(wifissid,wifipassword); 
 while (WiFi.status() != WL_CONNECTED ) 
          { 
          delay(500); 
           Serial.print(".");
          }
//**************NEED TO RUN ONCE IF YOU HAD NOT ADDED SENSORS ********************************
                   
                JsonObject sensor1 = json_body.createNestedObject();
                 sensor1["name"] = "Temperature";
                 sensor1["unit"] = "C";             //Optional
                JsonObject sensor2 = json_body.createNestedObject();
                 sensor2["name"] = "Humidity";
                 sensor2["unit"] =  "RH";           //Optional
   serializeJson(json_body, rbody); 
   Serial.print("addsensor GET requestbody:");Serial.println(rbody);
   AddSensor(rbody);
   json_body.clear();
   
//********************************************************************************************   
}

void loop() {
   temperature=(dht.readTemperature());
   humidity=(dht.readHumidity());
   json_body.clear();
   rbody=""; //This is necessary for serialization of json_body to String rbody.
   JsonObject sensor1 = json_body.createNestedObject();
                 sensor1["name"] = "Temperature";
                 sensor1["value"] = temperature;
                 sensor1["unit"] = "C";             
   JsonObject sensor2 = json_body.createNestedObject();
                 sensor2["name"] = "Humidity";
                 sensor2["value"] = humidity;
                 sensor2["unit"] =  "RH";
                  
    serializeJson(json_body, rbody);
    Serial.print("updatesensor POST requestbody:");Serial.println(rbody);
    UpdateSensor(rbody);
     delay(2000);
    get_control_signal();
     delay(2000);
  
}   


void AddSensor(String rbody)
 {
  http.begin("http://api.cavyiot.com/"+api_key+"/sensor/add"); 
  int httpCode = http.POST(rbody);
  if(httpCode<0){
    http.end();
     Serial.println("...Server busy.. Trying to attempt again please wait.. ");
     AddSensor(rbody);
    }
  else{
     Serial.println("Response of adds_sensor:");
     Serial.println(http.getString());
     http.end();
    }
  }
void UpdateSensor(String rbody)
 {
  http.begin("http://api.cavyiot.com/"+api_key+"/sensor/update"); 
  int httpCode = http.POST(rbody);
   if(httpCode<0)
       {
        Serial.println("Trying to attempt again please wait.. Check Your WiFi/Internet connection.");//
        }
   else{
        String payload=http.getString();
        Serial.println("Response of update/sensor:");
        Serial.println(payload);
        }
     http.end();
    }
 void get_control_signal()
   {
   http.begin("http://api.cavyiot.com/"+api_key+"/button/status"); //
  int httpCode = http.GET();
  if(httpCode<0){
     Serial.println("coonection ERROR......... ");
      }
  else{
     Serial.println("Response of API GET  button/status:");
     String payload=http.getString();
      Serial.println(payload);//DEBUG
      payload.trim();
      Serial.println("Deserializing response String:");
      DeserializationError error = deserializeJson(json_body,payload);
      if(error)
         {
          Serial.println("Invalid Response.");//occures only when responce is not JSON String.
          
         }
      else
         {
          Serial.println("Response String is a valid JSONstring.");//occures only when responce is not JSON String.
          switch((int)json_body["code"]) 
          {
          case 425:
          Serial.println("To Early. Waiting for waiting_millis recevied form server");
                   
          //UpdateSensorCallMillis=millis()+(int)json_body["wait_for_millis"];
          break;
          case 200:
          Serial.println("200 OK");
         if( ProcessControlSignal(json_body["buttons"]))
          {
          Serial.println("STATE of GPIO pins are updated According to control signals!");
          };
          break;
          }
          
         }    
    
   }
     http.end();
   
 } 
bool ProcessControlSignal(JsonArray buttons)
{
Serial.print("Sizeof Button Array:");Serial.println(buttons.size());
for(int i=0;i<buttons.size();i++)
    {
          if((int)buttons[i]["index"]==0 && buttons[i]["state"]=="ON") {digitalWrite(BUTTON_0, LOW);} 
          if((int)buttons[i]["index"]==0 && buttons[i]["state"]=="OFF"){digitalWrite(BUTTON_0, HIGH);}
          if((int)buttons[i]["index"]==1 && buttons[i]["state"]=="ON") {digitalWrite(BUTTON_1, LOW);} 
          if((int)buttons[i]["index"]==1 && buttons[i]["state"]=="OFF"){digitalWrite(BUTTON_1, HIGH);}
          if((int)buttons[i]["index"]==2 && buttons[i]["state"]=="ON") {digitalWrite(BUTTON_2, LOW);} 
          if((int)buttons[i]["index"]==2 && buttons[i]["state"]=="OFF"){digitalWrite(BUTTON_2, HIGH);}
          if((int)buttons[i]["index"]==3 && buttons[i]["state"]=="ON") {digitalWrite(BUTTON_3, LOW);} 
          if((int)buttons[i]["index"]==3 && buttons[i]["state"]=="OFF"){digitalWrite(BUTTON_3, HIGH);}

           
    }
   
    return(true);
    
 }
       
