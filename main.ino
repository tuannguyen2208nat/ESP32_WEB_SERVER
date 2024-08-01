
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Wire.h>
#include <DHT20.h>
#include "config.h"

DHT20 dht20;
int led = D7;
AsyncWebServer server(httpPort);
AsyncWebSocket ws("/ws");

// Task to read temperature and humidity
void TaskTemperatureHumidity(void *pvParameters);

void setup() {
  Serial.begin(115200);
  pinMode(led,OUTPUT);

  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start DHT sensor
  dht20.begin();

  // Set up WebSocket events
  ws.onEvent(onEvent);
  server.addHandler(&ws);

  // Serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  });

  // Serve the JavaScript file
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/script.js", "application/javascript");
  });

  // Serve the CSS file
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/styles.css", "text/css");
  });

  // Start server
  server.begin();

  // Start the temperature and humidity task
  xTaskCreate(TaskTemperatureHumidity, "TaskTemperatureHumidity", 2048, NULL, 2, NULL);
  Serial.println("Task started");
}

void loop() {
  // The main loop can be used for other tasks if necessary
}

void TaskTemperatureHumidity(void *pvParameters) {
  while (1) {
    // Read data from the DHT20 sensor
    dht20.read();
    float temperature = dht20.getTemperature();
    float humidity = dht20.getHumidity();
    if (ws.count() > 0) {
      String data = String("{\"temperature\":") + temperature + ",\"humidity\":" + humidity + "}";
      ws.textAll(data);
    }

    // Wait for 5 seconds before reading again
    delay(5000);
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0;
      Serial.printf("Received data: %s\n", (char*)data);
      if (strcmp((char*)data, "toggleON") == 0) {
        digitalWrite(led, HIGH);
        Serial.println("Turn LED ON");
      } else if (strcmp((char*)data, "toggleOFF") == 0) {
        digitalWrite(led, LOW);
        Serial.println("Turn LED OFF");
      }
    }
  }
}