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

#define MIN_AMB_BRIGHTNESS 3000

struct NodeInfo {
  uint32_t id;
  int32_t x;
  int32_t y;
  uint32_t brightness;
  bool detectedMotion;
  bool locked;
};

bool neighbours[8] = {false};

NodeInfo info;
painlessMesh  mesh;
Scheduler     scheduler;


int lastReading = 4095;

int currentState   = LOW;
int previousState  = LOW;

bool isNeighbour(int32_t x, int32_t y) {
  return (abs(info.x - x) < 2 && abs(info.y - y) < 2) && !(info.x == x && info.y == y);
}

void turnOff();
void sendKeepAliveMessage();
void sendChangeBrightnessMessage();
void sendSwitchMessage(int newStatus);
void sendLockMessage(int action, int brightness);

Task taskTurnOff(TASK_SECOND * 20, TASK_ONCE, &turnOff);
Task taskSendKeepAliveMessage(TASK_MINUTE * 5, TASK_FOREVER, &sendKeepAliveMessage);

void turnOff() {
  info.detectedMotion = false;
  sendSwitchMessage(0);
}

void sendLockMessage(int action, int brightness=0) {
  JSONVar lockMessage;
  lockMessage["type"] = 0;
  lockMessage["action"] = action;
  lockMessage["brightness"] = brightness;
  mesh.sendBroadcast(JSON.stringify(lockMessage), true);
}

void sendKeepAliveMessage() {
  JSONVar keepAliveMessage;
  keepAliveMessage["type"] = 1;
  keepAliveMessage["action"] = 0;
  keepAliveMessage["id"] = info.id;
  keepAliveMessage["x"] = info.x;
  keepAliveMessage["y"] = info.y;

  String jsonString = JSON.stringify(keepAliveMessage);
  mesh.sendBroadcast(jsonString);
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
  mesh.sendBroadcast(jsonString);
  Serial.println(jsonString);
}

void sendSwitchMessage(int newStatus) {
  JSONVar switchMessage;
  switchMessage["type"] = 2;
  switchMessage["id"] = info.id;
  switchMessage["x"] = info.x;
  switchMessage["y"] = info.y;
  switchMessage["status"] = newStatus;

  String jsonString = JSON.stringify(switchMessage);
  mesh.sendBroadcast(jsonString);
}

void receivedCallback(uint32_t from, String &msg) {
  JSONVar receivedMessage = JSON.parse(msg);

  if (int(receivedMessage["type"]) == 0) {
    if (int(receivedMessage["action"]) == 0) {
      info.brightness = 0;
      info.locked = false;
    } else if (int(receivedMessage["action"]) == 1) {
      info.brightness = int(receivedMessage["brightness"]);
      info.locked = true;
    }
  } else if (int(receivedMessage["type"]) == 1) {
    Serial.println(msg);
  } else if (int(receivedMessage["type"]) == 2) {
    int32_t x = int(receivedMessage["x"]) - info.x + 1;
    int32_t y = int(receivedMessage["y"]) - info.y + 1;
    size_t index = x + y * 3;
    if (index > 3) index--;

    if (isNeighbour(int(receivedMessage["x"]), int(receivedMessage["y"]))) {
      if (int(receivedMessage["status"]) == 0) {
        neighbours[index] = false;
      } else if (int(receivedMessage["status"]) == 1) {
        neighbours[index] = true;
      }
    }
  }
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

  info.id = mesh.getNodeId();
  info.x = 1;
  info.y = 0;
  info.brightness = 0;
  info.detectedMotion = false;
  info.locked = false;

  scheduler.addTask(taskTurnOff);
  scheduler.addTask(taskSendKeepAliveMessage);
  taskTurnOff.enable();
  taskSendKeepAliveMessage.enable();
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    JSONVar receivedMessage = JSON.parse(input);
    if (int(receivedMessage["type"]) == 0) {
      if (int(receivedMessage["action"]) == 0) {
        sendLockMessage(int(receivedMessage["action"]));
      } else if (int(receivedMessage["action"]) == 1){
        sendLockMessage(int(receivedMessage["action"]), int(receivedMessage["brightness"]));
      }
    }
  }
  previousState = currentState;
  currentState = digitalRead(PIR_PIN);
  lastReading = analogRead(LIGHT_SENSOR_PIN);
  if (previousState == LOW && currentState == HIGH) {
    sendSwitchMessage(1);
    info.detectedMotion = true;
    taskTurnOff.restartDelayed(20 * TASK_SECOND);
  }

  controlLigths();
  mesh.update();
}
