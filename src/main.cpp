#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Ticker.h>
#include <stdlib.h>
/* Macros */
#define SSID "Radiant"
#define PASS "123456789Log"
int ClientsCounter = 0;

/* Variables */
struct Relay {
    uint8_t pin;
    bool state;
    void update() {
        digitalWrite(pin, state ? LOW : HIGH); // Relay is active low
    }
};
Relay Lamp1 = {D0, false};
Relay Fan = {D1, false};
int interval = 1000;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
Ticker t;
Ticker t2;
/* Functions */
void WEBSOCKET_NotifyClients() {
    Serial.println("A Change Happened!! Notifying all clients..");
        ws.textAll(Lamp1.state ? "Lamp1_ON" : "Lamp1_OFF");
        ws.textAll(Fan.state ? "Fan_ON" : "Fan_OFF");
}
char *lastN(const char *str, size_t n)
{
    size_t len = strlen(str);
    return (char *)str + len - n;
}
void lamp_turn_off(){
    Lamp1.state=false;
    Lamp1.update();
    WEBSOCKET_NotifyClients();
}
void WEBSOCKET_HandleMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        Serial.printf("%s\n",lastN((char *)data,2));
        if (strcmp((char *)data, "Lamp1") == 0) {
            Lamp1.state = !Lamp1.state;
            digitalWrite(Lamp1.pin, Lamp1.state ? 0 : 1);
            WEBSOCKET_NotifyClients();
        } else if (strcmp((char *)data, "Fan") == 0) {
            Fan.state = !Fan.state;
            digitalWrite(Fan.pin, Fan.state ? 0 : 1);
            WEBSOCKET_NotifyClients();
        }
        else{
            t2.once( strtod(lastN((char *)data,2),NULL), lamp_turn_off);
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
        // int a=millis();
        //ws.text(client->id(), String(millis()));
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
        Serial.println("Pong");
    case WS_EVT_ERROR:
        break;
    }
}

void WEBSERVER_SendUpTime() {
    ws.textAll(String(millis()));
}
String WEBSERVER_processor(const String &var) {
    Serial.println(var);
    if (var == "Lamp1_STATE") {
        Serial.println("Found the Lamp1_STATE Variable, Now Replacing it...");
        if (Lamp1.state)
            return "ON";
        else
            return "OFF";
    }
    if (var == "Fan_STATE") {
        Serial.println("Found the Fan_STATE Variable, Now Replacing it...");
        if (Fan.state)
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
    pinMode(Lamp1.pin, OUTPUT);
    pinMode(Fan.pin, OUTPUT);
    digitalWrite(Lamp1.pin,1);
    digitalWrite(Fan.pin,1);

    Serial.begin(115200);
    delay(500);

    WIFI_Init();

    SPIFFS_Init();

    WEBSOCKET_Init();
    WEBSERVER_Init();
    t.attach(1, WEBSERVER_SendUpTime);
}

void loop() {
    ws.cleanupClients();
    // ws.textAll("Sensor Data");
    // delay(250);
}