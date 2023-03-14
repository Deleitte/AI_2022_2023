#include "painlessMesh.h"
#include "Arduino_JSON.h"
#include "WiFi.h"

#define   SSID       "starGazer"
#define   PASSWORD   "12345678910"
#define   PORT       6969

#define LIGHT_SENSOR_PIN  34
#define PIR_PIN 35
#define LED_PIN 33

struct NodeInfo {
  uint32_t id;
  int32_t x;
  int32_t y;
  boolean isOn;
  uint32_t brightness;
};

NodeInfo info;

painlessMesh  mesh;
Scheduler     scheduler;

int lastReading = 4095;

int pinStateCurrent   = LOW;
int pinStatePrevious  = LOW;

void turnOff();
void sendChangeBrightnessMessage(int brightness);
void sendMotionDetectedMessage();

Task taskTurnOff( TASK_SECOND * 20 , TASK_FOREVER, &turnOff );

void turnOff() {
  sendChangeBrightnessMessage(0);
  info.isOn = false;
  //Serial.println("Turn off");
  ledcWrite(0, 0);
}

void receivedCallback(uint32_t from, String &msg) {
  Serial.println(msg);
  JSONVar receivedMessage = JSON.parse(msg);
  if (int(receivedMessage["type"]) == 1 && int(receivedMessage["x"]) - info.x < 2 && int(receivedMessage["y"]) - info.y < 2) {
    if (info.isOn == false) {
      sendChangeBrightnessMessage(5);
      //Serial.println("Received turn on message");
    }
    ledcWrite(0, int(255*info.brightness/100));
    taskTurnOff.setInterval( TASK_SECOND * 20 );
  }
  //Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
    //Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  //Serial.printf("Changed connections\n");  Serial.println

}

void sendChangeBrightnessMessage(int brightness) {
  info.brightness = brightness;
  JSONVar changeBrightMessage;
  changeBrightMessage["id"] = info.id;
  changeBrightMessage["x"] = info.x;
  changeBrightMessage["y"] = info.y;
  changeBrightMessage["brightness"] = brightness;
  changeBrightMessage["type"] = 2;

  String jsonString = JSON.stringify(changeBrightMessage);
  mesh.sendBroadcast(jsonString);
  Serial.println(jsonString);
}

void sendMotionDetectedMessage() {
  JSONVar motionDetectedMessage;
  motionDetectedMessage["id"] = info.id;
  motionDetectedMessage["x"] = info.x;
  motionDetectedMessage["y"] = info.y;
  motionDetectedMessage["type"] = 1;

  String jsonString = JSON.stringify(motionDetectedMessage);
  mesh.sendBroadcast(jsonString);
  Serial.println(jsonString);
}

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  ledcSetup(0, 5000, 8);
  ledcAttachPin(LED_PIN, 0);

  mesh.setDebugMsgTypes( ERROR | STARTUP );

  mesh.init(SSID, PASSWORD, &scheduler, PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);

  scheduler.addTask(taskTurnOff);
  taskTurnOff.enable();

  info.id = mesh.getNodeId();
  info.x = 1;
  info.y = 0;
  info.isOn = false;
  info.brightness = 0;

  pinMode(LIGHT_SENSOR_PIN, INPUT);
}

void loop() {
  /*
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    if (input == "turn on") {
      //digitalWrite(LED_PIN, HIGH);
      taskTurnOff.setInterval( TASK_SECOND * 20 );
    }
    else if (input == "turn off") {
      digitalWrite(LED_PIN, LOW);
    }
  }
  */
  pinStatePrevious = pinStateCurrent;
  pinStateCurrent = digitalRead(PIR_PIN);
  lastReading = analogRead(LIGHT_SENSOR_PIN);
  
  if (pinStatePrevious == LOW && pinStateCurrent == HIGH) {
    //Serial.println("Motion detected!");
    if (lastReading < 2048) {
      sendMotionDetectedMessage();
      info.isOn = true;
      info.brightness = 100;
      ledcWrite(0, 255);
      taskTurnOff.setInterval( TASK_SECOND * 20 );
    }
  }
  else{
  if (pinStatePrevious == HIGH && pinStateCurrent == LOW) {
    //Serial.println("Motion stopped!");
    //digitalWrite(LED_PIN, LOW);
  }}
  mesh.update();
}
