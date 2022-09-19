#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

/* Macros */
#define SSID "Radiant"
#define PASS "123456789Log"
int ClientsCounter = 0;
/* Variables */
struct Led {
    uint8_t pin;
    bool on;

    void update() {
        digitalWrite(pin, on ? HIGH : LOW);
    }
};
Led led = {D0, true};

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

/* Functions */
void WEBSOCKET_NotifyClients() {
    Serial.println("A Change Happened!! Notifying all clients..");
    ws.textAll(led.on ? "ON" : "OFF");
}

void WEBSOCKET_HandleMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        if (strcmp((char *)data, "Lamp1") == 0) {
            led.on = !led.on;
            digitalWrite(led.pin, led.on ? 1 : 0);
            WEBSOCKET_NotifyClients();
        }
    }
}

void WEBSOCKET_OnEvent(AsyncWebSocket *server,
                       AsyncWebSocketClient *client,
                       AwsEventType type,
                       void *arg,
                       uint8_t *data,
                       size_t len) {
    switch (type) {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket Client Connected :) ID:%u ,Connected from:%s\n", client->id(), client->remoteIP().toString().c_str());
        ClientsCounter++;
        Serial.printf("Clients Connected Now: %d\n", ClientsCounter);
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket Client Disconnected :( ID:%u\n", client->id());
        ClientsCounter--;
        Serial.printf("Clients Connected Now: %d\n", ClientsCounter);
        break;
    case WS_EVT_DATA:
        Serial.println("Received a massage from a client :) Handling it...");
        WEBSOCKET_HandleMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

String WEBSERVER_processor(const String &var) {
    Serial.println(var);
    if (var == "STATE") {
        Serial.println("Found the STATE Variable, Now Replacing it...");
        if (led.on)
            return "ON";
        else
            return "OFF";
    }
    return "NONE";
    // return String(var == "STATE" && led.on ? "true" : "false");
}
void WEBSERVER_OnRootRequest(AsyncWebServerRequest *request) {
    Serial.println("Webserver Initilazation Continuing...");
    request->send(SPIFFS, "/index.html", "text/html", false, WEBSERVER_processor);
}
void WEBSERVER_Init() {
    Serial.println("Initializing Webserver..");
    server.on("/", WEBSERVER_OnRootRequest);
    server.serveStatic("/", SPIFFS, "/");
    Serial.println("Starting Server");
    server.begin();
}
void WEBSOCKET_Init() {
    Serial.println("Initilaizing WebSocket Protocol, Waiting for an event...");
    ws.onEvent(WEBSOCKET_OnEvent);
    server.addHandler(&ws);
}
void SPIFFS_Init() {
    if (!SPIFFS.begin()) {
        Serial.println("Cannot mount SPIFFS volume...");
        while (1)
            digitalWrite(LED_BUILTIN, millis() % 200 < 50 ? HIGH : LOW);
    }
    Serial.println("Mounted SPIFFS volume successfuly");
}
void WIFI_Init() {
    Serial.print("Connecting to " SSID);
    WiFi.begin(SSID, PASS);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.println("Connected to " SSID);
    Serial.println("IP Taken: " + WiFi.localIP().toString());
}

void setup() {
    pinMode(led.pin, OUTPUT);

    Serial.begin(115200);
    delay(500);

    WIFI_Init();

    SPIFFS_Init();

    WEBSOCKET_Init();
    WEBSERVER_Init();
}

void loop() {
    ws.cleanupClients();
}