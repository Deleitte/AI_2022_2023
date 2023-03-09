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
  int id;
  int x_pos;
  int y_pos;
};

NodeInfo info;

painlessMesh  mesh;
Scheduler     scheduler;
JSONVar turnOnMessage;

int lastReading = 4095;

int pinStateCurrent   = LOW;
int pinStatePrevious  = LOW;

void turnOff();

Task taskTurnOff( TASK_SECOND * 20 , TASK_FOREVER, &turnOff );

void turnOff() {
  Serial.println("Turn off");
  digitalWrite(LED_PIN, LOW);
}

void receivedCallback(uint32_t from, String &msg) {
  JSONVar receivedMessage = JSON.parse(msg);
  if (receivedMessage["type"].operator int() == 0 && int(receivedMessage["x_pos"]) - info.x_pos < 2 && int(receivedMessage["y_pos"]) - info.y_pos < 2) {
    Serial.println("Received turn on message");
    taskTurnOff.setInterval( TASK_SECOND * 20 );
    digitalWrite(LED_PIN, HIGH);
  }
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void sendTurnOnMessage() {
  turnOnMessage["id"] = info.id;
  turnOnMessage["x_pos"] = info.x_pos;
  turnOnMessage["y_pos"] = info.y_pos;
  turnOnMessage["type"] = 0;
  String jsonString = JSON.stringify(turnOnMessage);
  mesh.sendBroadcast(jsonString);
}

void setup() {
  Serial.begin(115200);

  info.id = 1;
  info.x_pos = 0;
  info.y_pos = 0;

  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  mesh.setDebugMsgTypes( ERROR | STARTUP );

  mesh.init(SSID, PASSWORD, &scheduler, PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);

  scheduler.addTask(taskTurnOff);
  taskTurnOff.enable();

  pinMode(LIGHT_SENSOR_PIN, INPUT);
}

void loop() {
  pinStatePrevious = pinStateCurrent;
  pinStateCurrent = digitalRead(PIR_PIN);
  lastReading = analogRead(LIGHT_SENSOR_PIN);

  if (pinStatePrevious == LOW && pinStateCurrent == HIGH) {
    Serial.println("Motion detected!");
    if (lastReading < 2048) {
      sendTurnOnMessage();
      digitalWrite(LED_PIN, HIGH);
      taskTurnOff.setInterval( TASK_SECOND * 20 );
    }
  }
  else{
  if (pinStatePrevious == HIGH && pinStateCurrent == LOW) {
    Serial.println("Motion stopped!");
    //digitalWrite(LED_PIN, LOW);
  }}
  mesh.update();
}
