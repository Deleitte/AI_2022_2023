#include "painlessMesh.h"
#include "Arduino_JSON.h"
#include "WiFi.h"

#define   SSID       "starGazer"
#define   PASSWORD   "12345678910"
#define   PORT       6969

#define LIGHT_SENSOR_PIN  34
#define PIR_PIN 35
#define LED_PIN_1 27
#define LED_PIN_2 26
#define LED_PIN_3 25
#define LED_PIN_4 33

#define MIN_AMB_BRIGHTNESS 1000

struct NodeInfo {
  uint32_t id;
  int32_t x;
  int32_t y;
  uint32_t brightness;
  bool detectedMotion;
};

bool neighbours[8] = {false};

NodeInfo info;
painlessMesh  mesh;
Scheduler     scheduler;


int lastReading = 4095;

int pinStateCurrent   = LOW;
int pinStatePrevious  = LOW;

bool isNeighbour(int32_t x, int32_t y) {
  return (abs(info.x - x) < 2 && abs(info.y - y) < 2);
}

void turnOff();
void sendChangeBrightnessMessage();
void sendMotionDetectedMessage();

Task taskTurnOff( TASK_SECOND * 20 , TASK_FOREVER, &turnOff );

void turnOff() {
  info.detectedMotion = false;
  Serial.println("turn off");
}

void sendChangeBrightnessMessage() {
  JSONVar changeBrightMessage;
  changeBrightMessage["id"] = info.id;
  changeBrightMessage["x"] = info.x;
  changeBrightMessage["y"] = info.y;
  changeBrightMessage["brightness"] = info.brightness;
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

void receivedCallback(uint32_t from, String &msg) {
  Serial.println("Received:" + msg);
  JSONVar receivedMessage = JSON.parse(msg);

  int32_t x = int(receivedMessage["x"]) - info.x + 1;
  int32_t y = int(receivedMessage["y"]) - info.y + 1;
  size_t index = x + y * 3;
  if (index > 3) index++;

  if (isNeighbour(int(receivedMessage["x"]), int(receivedMessage["y"]))) {
    if (int(receivedMessage["type"]) == 1) {
      neighbours[index] = true;
    } else if (int(receivedMessage["type"]) == 2 && int(receivedMessage["brigthness"]) == 0) {
      neighbours[index] = false;
    }
  }
  //Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void adaptBrightness(uint32_t brightness) {
  
  if (brightness != info.brightness) {
    info.brightness = brightness;
    sendChangeBrightnessMessage();
  } else {
    info.brightness = brightness;
  }
}

void controlLigths() {

  if (lastReading < MIN_AMB_BRIGHTNESS) {
    if (info.detectedMotion) {
      adaptBrightness(100);
    } else {
      bool hasNeighbourActive = false;
      for (int i = 0; i < 8; i++) {
        if (neighbours[i]) {
          hasNeighbourActive = true;
          break;
        }
      }
      if (hasNeighbourActive) {
        adaptBrightness(25);
      } else {
        adaptBrightness(0);
      }
    }
  } else {
    adaptBrightness(0);
  }
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, LOW);
  digitalWrite(LED_PIN_4, LOW);
  if (info.brightness >= 25) {
    digitalWrite(LED_PIN_1, HIGH);
  }
  if (info.brightness >= 50) {
    digitalWrite(LED_PIN_2, HIGH);
  }
  if (info.brightness >= 75) {
    digitalWrite(LED_PIN_3, HIGH);
  }
  if (info.brightness == 100) {
    digitalWrite(LED_PIN_4, HIGH);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);

  mesh.setDebugMsgTypes( ERROR | STARTUP );

  mesh.init(SSID, PASSWORD, &scheduler, PORT);
  mesh.onReceive(&receivedCallback);


  scheduler.addTask(taskTurnOff);
  taskTurnOff.enable();

  info.id = mesh.getNodeId();
  info.x = 1;
  info.y = 1;
  info.brightness = 0;
  info.detectedMotion = false;

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
    sendMotionDetectedMessage();
    info.detectedMotion = true;
    taskTurnOff.setInterval( TASK_SECOND * 20 );
  }
  else{
  if (pinStatePrevious == HIGH && pinStateCurrent == LOW) {
    //Serial.println("Motion stopped!");
    //digitalWrite(LED_PIN, LOW);
  }}

  controlLigths();
  mesh.update();
}
