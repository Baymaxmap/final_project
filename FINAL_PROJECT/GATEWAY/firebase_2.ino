//**********************************************************
#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include "ThingSpeak.h"
//***********************************************************

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "P 1709 CT3"
#define WIFI_PASSWORD "21111993"

unsigned long ChannelID = 2593173;
const char* myWriteAPIKey = "HBFQOHLEP6N08EQI";
WiFiClient client;

//#define WIFI_SSID "Tran Ba Thanh"
//#define WIFI_PASSWORD "05122002"

#define API_KEY "AIzaSyD6_teMzjG-yipXFQjdV_Aln_RQGA_x_Fg"
#define DATABASE_URL "https://gpstracker13052024-1-default-rtdb.firebaseio.com/" 

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

/*************************************************************************/

void setup(){
  Serial.begin(9600);
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

  String latitude="",longtitude="",str_temp="",str_humi="",str_co="",str_dust="";
  float latData=0.0, longData=0.0, temp, humi, coGas, fineDust;
  

  
void loop(){
  String input="";
 //GET THE LATITUDE AND LONGTITUDE
    // Nếu có dữ liệu từ máy tính
  if (Serial.available() > 0) {
    // Đọc dữ liệu từ máy tính
    input = Serial.readString();

    if(input.indexOf("latitude:")!=-1){
      int latStart = input.indexOf("latitude:") + 9;
      int latEnd = input.indexOf("\r\n", latStart);
      latitude = input.substring(latStart, latEnd);

      int longStart = input.indexOf("longtitude:") + 11;
      int longEnd = input.indexOf("\r\n", longStart);
      longtitude = input.substring(longStart, longEnd);
    }
    
    int tempStart = input.indexOf("temp:") + 5;
    int tempEnd = input.indexOf("\r\n", tempStart);
    str_temp = input.substring(tempStart, tempEnd);

    int humiStart = input.indexOf("humi:") + 5;
    int humiEnd = input.indexOf("\r\n", humiStart);
    str_humi = input.substring(humiStart, humiEnd);

    int coStart = input.indexOf("CO gas:") + 7;
    int coEnd = input.indexOf("\r\n", coStart);
    str_co = input.substring(coStart, coEnd);

    int dustStart = input.indexOf("dust:") + 5;
    int dustEnd = input.indexOf("\r\n", dustStart);
    str_dust = input.substring(dustStart, dustEnd);

    if(input.indexOf("latitude:")!=-1){
      latData=latitude.toFloat();
      longData=longtitude.toFloat();
    }
    temp=str_temp.toFloat();
    humi=str_humi.toFloat();
    coGas=str_co.toFloat();
    fineDust=str_dust.toFloat();
        
    // In dữ liệu đã nhận được
    Serial.println("data sent from laptop: \n" + input);
    if(input.indexOf("latitude:")!=-1){
      Serial.println(latData,5);
      Serial.println(longData,5);
    }
    Serial.println(temp,1);
    Serial.println(humi,1);
    Serial.println(coGas,1);
    Serial.println(fineDust,0);
  }
  delay(1000);
//*************************************************************


  if((millis() - sendDataPrevMillis > 30000 || sendDataPrevMillis == 0)){
     sendDataPrevMillis = millis();

/**********upload data of GPS onto firebase*************/
  if (Firebase.ready() && signupOK && (input.indexOf("latitude:")!=-1)){
    //sendDataPrevMillis = millis();
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setFloat(&fbdo, "gps_tracker2/Latitude", latData)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "gps_tracker2/Longtitude", longData)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }


/**********upload parameters of environment data onto thingspeak*************/
    ThingSpeak.setField(1, temp);
    ThingSpeak.setField(2, humi);
    ThingSpeak.setField(3, coGas);
    ThingSpeak.setField(4, fineDust);

    // Write all fields at once
    int checkThingSpeak = ThingSpeak.writeFields(ChannelID, myWriteAPIKey);

    if (checkThingSpeak == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(checkThingSpeak));
    }
  }
  Serial.println("************************************************************");
}
