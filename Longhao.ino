#include <ArduinoJson.h>
#include <ESP32Servo.h>

#define MAX_SIGNAL 2000
#define MIN_SIGNAL 1000

Servo esc;
Servo rudder;
int ESP_PIN = 2;
int rudder_PIN = 4;
int mtr = 1000;

bool LED[] = {false,false};
void setup() {
  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  Serial.begin(115200);
  esc.setPeriodHertz(50);
  rudder.setPeriodHertz(50);
  esc.attach(ESP_PIN);
  rudder.attach(rudder_PIN);
  while (!Serial.available());
  esc.writeMicroseconds(MAX_SIGNAL);
  delay(100);
  esc.writeMicroseconds(MIN_SIGNAL);
  Serial.print("{\"message\":\"start\"}\n");
  delay(100);
  ledtest(1);
}
void ledtest(int num) {
  int led = 0;
  if(num == 0){
    led = 12;
   } else if (num == 1){
    led = 13;
   }
  Serial.println(num);
  Serial.println(led);
  if(led && led != 0){
    if(LED[num] == true){
      digitalWrite(led,LOW);
      LED[num] = false;
    } else {
      digitalWrite(led,HIGH);
      LED[num] = true;
    }
  }
}

void loop() {
    char str[255];
    DynamicJsonDocument doc(1024);
    DynamicJsonDocument serialText(1024);
  if(Serial.available() > 0){
    DeserializationError err = deserializeJson(serialText, Serial.readStringUntil('\n'));
    if (err) {
      String errMsg = "Failed json parse: " + String(err.c_str());
      doc["isSuccess"] = false;
      doc["message"] = errMsg;
     } else {
      int motor = serialText["motor"];
      if(motor != mtr){
        if(motor != 0){
          if(motor >= 1000 && motor <= 2000){
            mtr = motor;
            esc.writeMicroseconds(motor);
            doc["isSuccess"] = true;
            doc["message"] = "Success";
            doc["value"] = motor; 
          } else {
            doc["isSuccess"] = false;
            doc["message"] = "Motor is not in range.";
            if(motor == 114514){
              ledtest(0);
              doc["isSuccess"] = true;
              doc["message"] = "514514";
              doc["value"] = motor; 
            }
            if(motor == 123456){
              ledtest(1);
              doc["isSuccess"] = true;
              doc["message"] = "haode";
              doc["value"] = motor; 
            }
          }
        } else {
           doc["isSuccess"] = false;
           doc["message"] = "Motor a Int or Value is 0.";
        }
      } else {
         doc["isSuccess"] = false;
         doc["message"] = "Duplicate Motor Value."; 
      }
    }
   serializeJson(doc, Serial);
   Serial.print("\n");
 }
 // String sensorValue = "Hello!";
 // Serial.println(sensorValue);
}
