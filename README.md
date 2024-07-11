# ESP32-Smart-Home

This project is an ESP32-based smart home system that allows you to control a lamp and a fan remotely via a web interface using WebSockets. It also provides real-time updates on the server's uptime.

## Features

- Control any device like a lamp or a fan remotely.
- Real-time updates using WebSockets.
- Displays server uptime.
- User-friendly web interface.

## Project Structure

```
ESP32-Smart-Home/
├── data/
│   ├── index.css
│   ├── index.html
│   └── index.js
└── src/
    └── main.cpp
```

### data/index.html

This file contains the HTML structure for the web interface. It displays the current time, server uptime, and provides buttons to toggle the lamp and the fan.

### data/index.css

This file contains the CSS styling for the web interface. It ensures a user-friendly and responsive design.

### data/index.js

This file contains the JavaScript code for the web interface. It handles the WebSocket connection, updates the UI based on server messages, and sends toggle commands to the server.

### src/main.cpp

This file contains the main code for the ESP32. It sets up the Wi-Fi connection, initializes the file system, WebSocket server, and HTTP server. It also handles WebSocket events and controls the state of the lamp and the fan.

## Setup

1. **Install Required Libraries**

   Ensure you have the following libraries installed in your Arduino IDE:
   - ESP8266WiFi
   - ESPAsyncWebServer
   - FS
   - Ticker

2. **Configure Wi-Fi**

   Update the `SSID` and `PASS` macros in `src/main.cpp` with your Wi-Fi network's SSID and password.

   ```cpp
   #define SSID "Your_SSID"
   #define PASS "Your_PASSWORD"
   ```

3. **Upload Filesystem Image**

   Use the Arduino IDE to upload the files in the `data/` folder to the ESP32's filesystem. This can be done using the "ESP32 Sketch Data Upload" plugin.

4. **Upload Code**

   Upload the code in `src/main.cpp` to your ESP32 using the Arduino IDE.

5. **Access the Web Interface**

   Once the ESP32 is connected to your Wi-Fi network, it will print its IP address to the serial monitor. Open a web browser and navigate to this IP address to access the web interface.

## Usage

- Open the web interface in your browser.
- The interface displays the current time and server uptime.
- Use the "Toggle" buttons to turn the lamp and fan on or off.
- The state of the lamp and fan will be updated in real-time.
