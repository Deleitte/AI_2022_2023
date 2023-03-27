# StarGazer
#### Ambient Inteligence 2022/2023 
#### Group 2, MEIC-A

# Table of Contents
- [Introduction](#introduction)
- [Electronic Material](#electronic-material)
- [Hardware](#hardware)
- [Software](#software)
- [How to run](#how-to-run)


# Introduction
This project implements a smart street lighting system that uses both ambient light and motion readings to control the intensity of street lights. The system also provide a web interface to control the lights and to monitor the system. It can include a variable number of stations but the listings below include the materials needed to build 3 stations.

# Electronic Material
The electronic material used in this project is the following:
- 3 PIR sensors
- 3 LDR sensors
- 3 10kΩ resistors
- 12 LEDs
- 12 220Ω resistors
- Jumpers

# Hardware
To build the project, you will need the following hardware:
- 3 ESP32
- 1 Raspberry Pi (or any other a machine capable of running a web server)

# Software
The software needed to run the project is the following:
- PlatformIO
- Docker
- Docker Compose

# How to run

### 1. Setup the ESP32's
The source code (available in ```station/src/main.cpp```) for setting up the ESP32's contains some configuration variables that you can change to fit your needs. Here is an example of the configuration variables:
```cpp
#define MIN_AMB_BRIGHTNESS 3000     // Treshold for turning on the lights
#define STATION_POS_X 0             // X coordinate of the station
#define STATION_POS_Y 0             // Y coordinate of the station
#define TURN_OFF_TIME 20            // Time in seconds to turn off the lights after the last motion detected
#define SSID       "starGazer"    // Name of the WiFi mesh network
#define PASSWORD   "starGazer"    // Password of the WiFi mesh network
#define PORT       6666           // Port of the WiFi mesh network
```

To upload the code to the ESP32's you can use the PlatformIO IDE Visual Studio Code extension and click the upload button after opening the ```station``` project.

Alternatively, you can use the PlatformIO Core to upload the code. 
After installing the PlatformIO Core, you need to run the following command inside the ```station``` folder:
```
source ~/.platformio/penv/bin/activate
pio run --target upload
```

### 2. Run the server
To run the server you need to have Docker and Docker Compose installed.
You need also to connect one of the ESP32's to the machine running the server to provide serial communcation between the ESP32's and the server.

To run the web server, database and frontend you should run inside the root folder:
```
docker compose up -d -f docker-compose-esp.yml
```
You should now be able to access the web dashboard at ```http://localhost```
