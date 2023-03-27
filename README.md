# StarGazer
## Ambient Inteligence 2022/2023
## Group 2, MEIC-A

## How to run
### Prerequisites
- PlatformIO
  - Both PlatformIO Core and PlatformIO IDE should work
- Docker and Docker Compose

### Setup the ESP32's
With the PlatformIO IDE Visual Studio Code extension, 
you can open the stations folder and press the upload button.

If you prefer, you can use the PlatformIO Core to upload the code and for that you need to
run the following command inside the stations folder:
```
pio run --target upload
```

### Running the server
You need an ESP32 connected to the machine running the server to be able to have them communicate.
To run the server you should only need to run:
```
docker compose up -d -f docker-compose-esp.yml
```
