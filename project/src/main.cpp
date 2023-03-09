#include "painlessMesh.h"
#include "ArduinoJson.h"
#include "WiFi.h"

#define   SSID       "starGazer"
#define   PASSWORD   "12345678910"
#define   PORT       6969

#define LIGHT_SENSOR_PIN  34
#define PIR_PIN 35
#define LED_PIN 33

Scheduler scheduler; 
painlessMesh  mesh;

int lastReading = 4095;

int pinStateCurrent   = LOW;
int pinStatePrevious  = LOW;

void sendMessage();

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
  String msg = "Ligthing: ";
  msg = analogRead(LIGHT_SENSOR_PIN);
  mesh.sendBroadcast(msg);
  taskSendMessage.setInterval( TASK_SECOND );
}

void receivedCallback(uint32_t from, String &msg) {
  sscanf(msg.c_str(), "Ligthing: %d", &lastReading);
  
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  mesh.setDebugMsgTypes( ERROR | STARTUP );

  mesh.init(SSID, PASSWORD, &scheduler, PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);

  scheduler.addTask(taskSendMessage);
  taskSendMessage.enable();

  pinMode(LIGHT_SENSOR_PIN, INPUT);
}

void loop() {
  pinStatePrevious = pinStateCurrent;
  pinStateCurrent = digitalRead(PIR_PIN);
  lastReading = analogRead(LIGHT_SENSOR_PIN);

  if (pinStatePrevious == LOW && pinStateCurrent == HIGH) {
    Serial.println("Motion detected!");
    if (lastReading < 2048)
      digitalWrite(LED_PIN, HIGH);
  }
  else
  if (pinStatePrevious == HIGH && pinStateCurrent == LOW) {   // pin state change: HIGH -> LOW
    Serial.println("Motion stopped!");
    digitalWrite(LED_PIN, LOW);
  }

  mesh.update();
}
