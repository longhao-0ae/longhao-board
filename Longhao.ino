#include <ArduinoJson.h>
#include <ESP32Servo.h>

#define MAX_SIGNAL 2000
#define MIN_SIGNAL 1000

Servo esc;
Servo rudder;
int ESP_PIN = 2;
int rudder_PIN = 4;
int mtr = 1000;
int savedhelm = 0;

void setup() {
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  Serial.begin(115200);
  esc.setPeriodHertz(50);
  //rudder.setPeriodHertz(50);
  esc.attach(ESP_PIN);
  rudder.attach(rudder_PIN);
  while (!Serial.available());
  esc.writeMicroseconds(MAX_SIGNAL);
  delay(100);
  esc.writeMicroseconds(MIN_SIGNAL);
  Serial.print("{\"message\":\"start\"}\n");
  delay(100);
}

void loop() {
  char str[255];
  DynamicJsonDocument doc(1024);
  DynamicJsonDocument serialText(1024);
  if (Serial.available() > 0) {
    DeserializationError err = deserializeJson(serialText, Serial.readStringUntil('\n'));
    if (err) {
      String errMsg = "Failed json parse: " + String(err.c_str());
      doc["isSuccess"] = false;
      doc["message"] = errMsg;
    } else {
      /*
      serializeJson(serialText, Serial);
      Serial.print("\n");
      */
      if (serialText["helm"].isNull() == 0) {
        doc["helm"]  = Controlrudder(serialText["helm"].as<int>());
      }
      if(serialText["motor"].isNull() == 0){
        doc["motor"] = Controlmotor(serialText["motor"].as<int>());
        }
      doc["isSuccess"] = true;
    }
    serializeJson(doc, Serial);
    Serial.print("\n");
  }
}

DynamicJsonDocument Controlrudder(int helm) {
  DynamicJsonDocument doc(1024);
    if (helm != savedhelm) {
      if (helm >= 0 && helm <= 180) {
        savedhelm = helm;
        rudder.write(helm);
        doc["isSuccess"] = true;
        doc["message"] = "Success";
        doc["value"] = helm;
      } else {
        doc["isSuccess"] = false;
        doc["message"] = "Helm is not in range.";
      }
    } else {
      doc["isSuccess"] = false;
      doc["message"] = "Duplicate Helm Value.";
    }
  return doc;
}
DynamicJsonDocument Controlmotor(int motor) {
  DynamicJsonDocument doc(1024);
    if (motor != mtr) {
      if (motor >= 1000 && motor <= 2000) {
        mtr = motor;
        esc.writeMicroseconds(motor);
        doc["isSuccess"] = true;
        doc["message"] = "Success";
        doc["value"] = motor;
      } else {
        doc["isSuccess"] = false;
        doc["message"] = "Motor is not in range.";
      }
    } else {
      doc["isSuccess"] = false;
      doc["message"] = "Duplicate Motor Value.";
    }
  return doc;
}
