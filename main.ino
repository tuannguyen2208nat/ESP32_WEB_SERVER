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

bool buttonStates[4] = {false, false, false, false};

// Hàm để đọc nhiệt độ và độ ẩm
void TaskTemperatureHumidity(void *pvParameters);
void TaskSwitchRelay(void *pvParameters);

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    } else if (type == WS_EVT_DISCONNECT) {
         Serial.printf("WebSocket client #%u disconnected\n", client->id());
    } else if (type == WS_EVT_DATA) {
        String message = String((char *)data);
        if (message.startsWith("toggle")) {
            int buttonIndex = message.substring(6).toInt();
            Serial.println(buttonIndex);
            buttonStates[buttonIndex] = !buttonStates[buttonIndex];
            String stateMsg = "state" + String(buttonIndex) + ":" + (buttonStates[buttonIndex] ? "ON" : "OFF");
            ws.textAll(stateMsg);
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(led, OUTPUT);

    if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
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
        request->send(LittleFS, "/index.html", "text/html"); });
  // Serve the JavaScript file
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/script.js", "application/javascript"); });

  // Serve the CSS file
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/styles.css", "text/css"); });

  // Start server
  server.begin();

  // Start the temperature and humidity task
  xTaskCreate(TaskTemperatureHumidity, "TaskTemperatureHumidity", 2048, NULL, 2, NULL);
  xTaskCreate(TaskSwitchRelay, "TaskSwitchRelay", 2048, NULL, 2, NULL);
  Serial.println("Task started");
}

void loop() {
    // Vòng lặp chính có thể được sử dụng cho các nhiệm vụ khác nếu cần
}

void TaskTemperatureHumidity(void *pvParameters) {
    while (1) {
        // Đọc dữ liệu từ cảm biến DHT20
        dht20.read();
        float temperature = dht20.getTemperature();
        float humidity = dht20.getHumidity();
        if (ws.count() > 0) {
            String data = String("{\"temperature\":") + temperature + ",\"humidity\":" + humidity + ",\"states\":[";
            for (int i = 0; i < 4; i++) {
                data += (buttonStates[i] ? "\"ON\"" : "\"OFF\"");
                if (i < 3) data += ",";
            }
            data += "]}";
            ws.textAll(data);
        }

        // Đợi 5 giây trước khi đọc lại
        delay(5000);
    }
}

void TaskSwitchRelay(void *pvParameters) {
    pinMode(D3, OUTPUT);

    while (1) {
        // Điều khiển trạng thái relay dựa trên mảng buttonStates
        for (int i = 0; i < 4; i++) {
            if (buttonStates[i]) {
                digitalWrite(D3, HIGH);
            } else {
                digitalWrite(D3, LOW);
            }
        }
        // Đợi để tránh làm quá tải vòng lặp
        delay(100);
    }
}
