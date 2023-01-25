#include <Arduino.h>
#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClientSecureBearSSL.h>

const uint8_t fingerprint[20] = *FINGERPRINT HERE*;

ESP8266WiFiMulti WiFiMulti;

String payloadInfo;

int cycle;
int order;
int litLength;

int LED = D1;
int LED2 = D2;
int LED3 = D3;
int LED4 = D4;
int LED5 = D5;
int button = D6;
int buttonState = 0;

int jointLightCycle;
String buttonPushed;

bool jointRegimeActive;
long jointRegimeCycle;
int jointRegimeDirection;
int jointRegimeDistance;
bool jointRegimeReadsButtons;

int trafficLightNumber = 1;
bool trafficLightActive;
int trafficLightRegime;

bool trafficLight1Active;
int trafficLight1Cycle;
int trafficLight1Regime;

bool trafficLight2Active;
int trafficLight2Cycle;
int trafficLight2Regime;

bool trafficLight3Active;
int trafficLight3Cycle;
int trafficLight3Regime;

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  
  pinMode(button, INPUT);

  Serial.begin(9600);

  WiFi.mode(WIFI_STA);

  WiFiMulti.addAP("TLU", "");

  connection();
}

void loop() {
  connection();
  if((jointRegimeActive == 0) && (trafficLightActive == 1) && (trafficLightRegime == 1)){
    resetLEDs();
    regularCycle();
    Serial.println("JointRegime INACTIVE, Foor REGULAR");
  } else if ((jointRegimeActive == 0) && (trafficLightActive == 1) && (trafficLightRegime == 0)){
    resetLEDs();
    unregulatedCycle();
    Serial.println("JointRegime INACTIVE, Foor UNREGULATED");
  } else if (jointRegimeActive == 1) {
    resetLEDs();
    Serial.println("JoingRegime ACTIVE");
    jointRegime();
  } else {
    Serial.println("Foor INACTIVE");
    resetLEDs();
    connection();
  }
}

void resetLEDs(){
  digitalWrite(LED, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  digitalWrite(LED5, LOW);
}

void jointRegime() {
  Serial.println("JOINT REGIME ALGUSES");
  connection();
  if(trafficLightNumber == 1){
    if(jointRegimeDirection == 0){
      digitalWrite(LED3, HIGH);
      regularCycle();
      digitalWrite(LED3, HIGH);
      carLightGreen((jointRegimeDistance*2)+(jointLightCycle*2)+10000);
    } else {
      digitalWrite(LED3, HIGH);
      carLightGreen((jointLightCycle*2)+(jointRegimeDistance*2));
      regularCycle();
      digitalWrite(LED3, HIGH);
      carLightGreen(10000);
    }
  } else if(trafficLightNumber == 2){
    digitalWrite(LED3, HIGH);
    carLightGreen(jointLightCycle+jointRegimeDistance);
    regularCycle();
    digitalWrite(LED3, HIGH);
    carLightGreen(jointRegimeDistance+jointLightCycle+10000);
  } else if(trafficLightNumber == 3){
    if(jointRegimeDirection == 0){
      digitalWrite(LED3, HIGH);
      carLightGreen((jointLightCycle*2)+(jointRegimeDistance*2));
      regularCycle();
      digitalWrite(LED3, HIGH);
      carLightGreen(10000);
    } else {
      digitalWrite(LED3, HIGH);
      regularCycle();
      digitalWrite(LED3, HIGH);
      carLightGreen((jointRegimeDistance*2)+(jointLightCycle*2)+10000);
    }
  }
}

void regularCycle(){
  connection();
  digitalWrite(LED4, HIGH);
  digitalWrite(LED3, HIGH);
  
  carLightGreen(litLength);

  blinkingLed(LED3);

  digitalWrite(LED2, HIGH);
  delay(3000);

  switchLeds(LED2, LED, 500);

  switchLeds(LED4, LED5, litLength);

  blinkingLed(LED5);

  switchLeds(LED5, LED4, 500);

  switchLeds(LED, LED2, 3000);

  digitalWrite(LED2, LOW);
}

void carLightGreen(int timeLength){
  int remainder = timeLength % 100;
  int segments = (timeLength - remainder) / 100;

  for(int i = 0; i < segments; i++){
    buttonState = digitalRead(button);
    if(buttonState != 1){
      delay(100);
    } else if (buttonPushed == "true"){ 
      delay(5000);
      return;
    } else {
      if(jointRegimeActive == 1){
        buttonPressToDB("true");
        delay(5000);
        buttonPressToDB("false");
      }
      return;
    }
  }
  
  delay(remainder);
}

void unregulatedCycle(){
  digitalWrite(LED4, LOW);
  digitalWrite(LED5, LOW);

  do {
    connection();
    blinkingLed(LED2);
  } while (jointRegimeActive == 0 && trafficLightRegime == 0);

  digitalWrite(LED2, LOW);
}

void blinkingLed(int led){
  for(int i = 0; i < 3; i++){
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    delay(500);
  }
}

void switchLeds(int led, int led2, int delayTime){
  digitalWrite(led, LOW);
  digitalWrite(led2, HIGH);
  delay(delayTime);
}

void  tempSetup() {
  order = 1;
  cycle = 20000;

  if(cycle >= 15000){
    int remainder = cycle % 2;
    litLength = (cycle-remainder-13000)/2;
  } else {
    litLength = 150000-13000 / 2;
  }
}

void buttonPressToDB(String state){
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(fingerprint);

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, 
       "https://asjade-internet-e60a3-default-rtdb.europe-west1.firebasedatabase.app/1007/buttonPushed.json")) {  // HTTPS
      https.addHeader("Content-Type", "application/json");
      String teade;

      if(state == "true"){
        teade="{\"buttonPushed\":\"true\"}";
      } else {
        teade="{\"buttonPushed\":\"false\"}";
      }
    
      int httpCode = https.PUT(teade);

      Serial.printf("[HTTPS] GET2... code: %d\n", httpCode);

      if (httpCode > 0) {
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          Serial.println("HTTP Code OK");
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
}

void connection(){
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setFingerprint(fingerprint);
    HTTPClient https;

    if (https.begin(*client, "https://asjade-internet-e60a3-default-rtdb.europe-west1.firebasedatabase.app/1007.json")) {
      https.addHeader("Content-Type", "application/json");

      int httpCode = https.GET();

      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {

          String payload = https.getString();

          StaticJsonDocument<512> doc;

          DeserializationError error = deserializeJson(doc, payload);

          if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
          }

          const char* buttonPushed_buttonPushed = doc["buttonPushed"]["buttonPushed"];
          buttonPushed = buttonPushed_buttonPushed;

          JsonObject jointRegime = doc["jointRegime"];
          jointRegimeActive = jointRegime["active"];
          jointRegimeCycle = jointRegime["cycle"];
          jointRegimeDirection = jointRegime["direction"];
          jointRegimeDistance = jointRegime["distance"];
          jointRegimeReadsButtons = jointRegime["readsButtons"];

          int counter = 0;

          if(jointRegimeActive == 0){
            for (JsonObject trafficLight : doc["trafficLights"].as<JsonArray>()) {

              if(counter == 0) {
                trafficLight1Active = trafficLight["active"];
                trafficLight1Cycle = trafficLight["cycle"];
                trafficLight1Regime = trafficLight["regime"];
              } 
              if(counter == 1) {
                trafficLight2Active = trafficLight["active"];
                trafficLight2Cycle = trafficLight["cycle"];
                trafficLight2Regime = trafficLight["regime"];
              }
              if(counter == 2) {
                trafficLight3Active = trafficLight["active"];
                trafficLight3Cycle = trafficLight["cycle"];
                trafficLight3Regime = trafficLight["regime"];
              }

              counter += 1;
            }
            
            int cycleTemp;

            if(trafficLightNumber == 1){
              cycleTemp = trafficLight1Cycle;
              trafficLightActive = trafficLight1Active;
              trafficLightRegime = trafficLight1Regime;
            } else if (trafficLightNumber == 2){
              cycleTemp = trafficLight2Cycle;
              trafficLightActive = trafficLight2Active;
              trafficLightRegime = trafficLight2Regime;
            } else if (trafficLightNumber == 3){
              cycleTemp = trafficLight3Cycle;
              trafficLightActive = trafficLight3Active;
              trafficLightRegime = trafficLight3Regime;
            }

            if(trafficLight1Cycle >= 15000){
              cycle = trafficLight1Cycle;
            } else {
              cycle = 15000;
            }

            if(cycle >= 15000){
              int remainder = cycle % 2;
              litLength = (cycle-remainder-13000)/2;
            } else {
              litLength = 15000-13000 / 2; // siit eemaldasin 15000 ühe 0 ära
            }
          } else {
            int minimumCycleLength = 15000*3 + 2*jointRegimeDistance;

            if(jointRegimeCycle > minimumCycleLength){
              long remainder = (jointRegimeCycle-(2*jointRegimeDistance)) % 3;
              jointLightCycle = (jointRegimeCycle-(2*jointRegimeDistance)-remainder)/3;
            } else {
              jointLightCycle = 15000;
            }

            if(jointLightCycle >= 15000){
              int remainder = jointLightCycle % 2;
              litLength = (jointLightCycle-remainder-13000)/2;
            } else {
              litLength = 15000-13000 / 2;
            }
          }
        }
      }
      https.end();
    }
  }
}