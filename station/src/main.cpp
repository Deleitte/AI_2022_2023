#include "painlessMesh.h"
#include "Arduino_JSON.h"
#include "WiFi.h"

#define MIN_AMB_BRIGHTNESS 3000
#define STATION_POS_X 0
#define STATION_POS_Y 0

#define TURN_OFF_TIME 20

#define   SSID       "starGazer"
#define   PASSWORD   "12345678910"
#define   PORT       6969

#define LIGHT_SENSOR_PIN  34
#define PIR_PIN 35
#define LED_PIN_1 27
#define LED_PIN_2 26
#define LED_PIN_3 25
#define LED_PIN_4 33

struct NodeInfo {
  uint32_t id;
  int32_t x;
  int32_t y;
  uint32_t brightness;
  bool detectedMotion;
  bool locked;
};

NodeInfo info;
painlessMesh  mesh;
Scheduler     scheduler;

int lastReading = 4095;
int currentState   = LOW;
int previousState  = LOW;
bool neighbours[8] = {false};

void sendChangeBrightnessMessage();
void sendSwitchMessage();
void sendLockMessage(uint32_t id, int action, int brightness);

Task taskTurnOff(TASK_SECOND * TURN_OFF_TIME, TASK_ONCE, []() {
  info.detectedMotion = false;
});

Task taskCleanNeighbours(TASK_SECOND * 10, TASK_ONCE, []() {
  for (int i = 0; i < 8; i++) {
    neighbours[i] = false;
  }
});

Task taskSendKeepAliveMessage(TASK_MINUTE * 5, TASK_FOREVER, []() {
  JSONVar keepAliveMessage;
  keepAliveMessage["type"] = 1;
  keepAliveMessage["action"] = 0;
  keepAliveMessage["id"] = info.id;
  keepAliveMessage["x"] = info.x;
  keepAliveMessage["y"] = info.y;

  String jsonString = JSON.stringify(keepAliveMessage);
  mesh.sendBroadcast(jsonString);
});

void sendLockMessage(uint32_t id, int action, int brightness=0) {
  JSONVar lockMessage;
  lockMessage["type"] = 0;
  lockMessage["id"] = id;
  lockMessage["action"] = action;
  lockMessage["brightness"] = brightness;
  mesh.sendBroadcast(JSON.stringify(lockMessage), true);
}

void sendChangeBrightnessMessage() {
  JSONVar changeBrightMessage;
  changeBrightMessage["type"] = 1;
  changeBrightMessage["action"] = 1;
  changeBrightMessage["id"] = info.id;
  changeBrightMessage["x"] = info.x;
  changeBrightMessage["y"] = info.y;
  changeBrightMessage["brightness"] = info.brightness;
  changeBrightMessage["overrided"] = info.locked;

  String jsonString = JSON.stringify(changeBrightMessage);
  mesh.sendBroadcast(jsonString, true);
}

void sendSwitchMessage() {
  JSONVar switchMessage;
  switchMessage["type"] = 2;
  switchMessage["id"] = info.id;
  switchMessage["x"] = info.x;
  switchMessage["y"] = info.y;

  String jsonString = JSON.stringify(switchMessage);
  mesh.sendBroadcast(jsonString);
}

void adaptBrightness(uint32_t brightness) {
  if (brightness != info.brightness) {
    info.brightness = brightness;
    sendChangeBrightnessMessage();
  } else {
    info.brightness = brightness;
  }
}


bool isNeighbour(int32_t x, int32_t y) {
  return (abs(info.x - x) < 2 && abs(info.y - y) < 2) && !(info.x == x && info.y == y);
}

void receivedCallback(uint32_t from, String &msg) {
  JSONVar receivedMessage = JSON.parse(msg);

  if (int(receivedMessage["type"]) == 0) {
    if (int(receivedMessage["action"]) == 0 && (uint32_t(receivedMessage["id"]) == info.id || uint32_t(receivedMessage["id"]) == 0)) {
      info.locked = false;
      adaptBrightness(0);
    } else if (int(receivedMessage["action"]) == 1 && (uint32_t(receivedMessage["id"]) == info.id || uint32_t(receivedMessage["id"]) == 0)) {
      info.locked = true;
      adaptBrightness(int(receivedMessage["brightness"]));
    }
  } else if (int(receivedMessage["type"]) == 1) {
    Serial.println(msg);
  } else if (int(receivedMessage["type"]) == 2) {
    int32_t x = int(receivedMessage["x"]) - info.x + 1;
    int32_t y = int(receivedMessage["y"]) - info.y + 1;
    size_t index = x + y * 3;
    if (index > 3) index--;

    if (isNeighbour(int(receivedMessage["x"]), int(receivedMessage["y"]))) {
      neighbours[index] = true;
      taskCleanNeighbours.restartDelayed(TURN_OFF_TIME * TASK_SECOND);
    }
  }
}



void controlLigths() {
  if (info.locked) {
    adaptBrightness(info.brightness);
  } else if (lastReading < MIN_AMB_BRIGHTNESS) {
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
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  mesh.setDebugMsgTypes( ERROR | STARTUP );
  mesh.init(SSID, PASSWORD, &scheduler, PORT);
  mesh.onReceive(&receivedCallback);

  info.id = mesh.getNodeId()%1000000000;
  info.x = STATION_POS_X;
  info.y = STATION_POS_Y;
  info.brightness = 0;
  info.detectedMotion = false;
  info.locked = false;

  scheduler.addTask(taskTurnOff);
  scheduler.addTask(taskSendKeepAliveMessage);
  scheduler.addTask(taskCleanNeighbours);
  taskTurnOff.enable();
  taskSendKeepAliveMessage.enable();
  taskCleanNeighbours.enable();
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    JSONVar receivedMessage = JSON.parse(input);
    if (int(receivedMessage["type"]) == 0) {
      if (int(receivedMessage["action"]) == 0) {
        sendLockMessage(uint32_t(receivedMessage["id"]), int(receivedMessage["action"]));
      } else if (int(receivedMessage["action"]) == 1){
        sendLockMessage(uint32_t(receivedMessage["id"]), int(receivedMessage["action"]), int(receivedMessage["brightness"]));
      }
    }
  }
  previousState = currentState;
  currentState = digitalRead(PIR_PIN);
  lastReading = analogRead(LIGHT_SENSOR_PIN);
  if (previousState == LOW && currentState == HIGH) {
    sendSwitchMessage();
    info.detectedMotion = true;
    taskTurnOff.restartDelayed(TURN_OFF_TIME * TASK_SECOND);
  }

  controlLigths();
  mesh.update();
}
