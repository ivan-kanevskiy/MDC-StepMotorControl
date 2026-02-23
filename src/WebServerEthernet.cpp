#include "main.h"
#include "WebServerEthernet.h"
#include "htmlcode.h"
#include "encoder.h"
#include "motor.h"

AsyncWebServer server(80);
uint8_t mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x32, 0x06};
AsyncWebSocket ws("/ws");
#define led PC13

// Helper function to safely build and send the data string
void webServerBroadcastStatus()
{
    // Using String is safer here to prevent buffer overflows and handle floats natively
    String payload = "MSteps:" + String(StepsToMove) +
                     ",ESteps:" + String(raw_count) +
                     ",EDeg:" + String(constrained_angle, 2);
    ws.textAll(payload);
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    if (type == WS_EVT_CONNECT)
    {
#ifdef debug
        MySerial.println("Websocket client connected!");
#endif
    }
    else if (type == WS_EVT_DISCONNECT)
    {
#ifdef debug
        MySerial.println("Client disconnected");
#endif
    }
    else if (type == WS_EVT_DATA)
    {
        String message = "";
        for (size_t i = 0; i < len; i++)
        {
            message += (char)data[i];
        }
#ifdef debug
        MySerial.print("Received data from website: ");
        MySerial.println(message);
#endif
        // If website is setting the motor steps
        if (message.startsWith("SET_NUM:"))
        {
            String valueStr = message.substring(8);
            StepsToMove = valueStr.toInt();
#ifdef debug
            MySerial.print("Motor steps updated to: ");
            MySerial.println(StepsToMove);
#endif
            // Broadcast the update to all screens
            webServerBroadcastStatus();
        }
    }
}

void handleRoot(AsyncWebServerRequest *request)
{
    digitalWrite(led, 1);
    request->send(200, "text/html", HTML);
    digitalWrite(led, 0);
}

void handleNotFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "File Not Found");
}
bool LAN_connected = false;
void WebServerInit()
{
    LAN_connected = false;
    digitalWrite(led, 0);
#ifdef debug
    MySerial.println("Requesting IP from router via DHCP...");
#endif
    // Initialize Ethernet with DHCP by only passing the MAC address
    // This function returns 0 if it fails to get an IP
    if (Ethernet.begin(mac) == 0)
    {
#ifdef debug
        MySerial.println("Failed to configure Ethernet using DHCP!");
#endif
    }
    else
    {
        LAN_connected = true;
    }

    if (LAN_connected)
    {
        ws.onEvent(onWsEvent);
        server.addHandler(&ws);
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                  { handleRoot(request); });
        server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(200, "text/javascript", javascript); });
        server.onNotFound(handleNotFound);
        server.begin();
#ifdef debug
        MySerial.print(F("HTTP WebServer is @ IP : "));
        MySerial.println(Ethernet.localIP()); // Print the IP assigned by your router
#endif
    }
}

unsigned long lastWsUpdate = 0;

void webServerUpdateLoop()
{
    if (LAN_connected)
    {
        if (ws.count() > 0 && (millis() - lastWsUpdate > 100))
        {
            lastWsUpdate = millis();
            webServerBroadcastStatus();
        }
        ws.cleanupClients();
    }
}