#include <WiFi.h>
#include <WebServer.h>
#include <DHT20.h>
#include "config.h"

void TaskTemperatureHumidity(void *pvParameters);

WebServer server(httpPort);
DHT20 dht20;

// Variables to hold the temperature and humidity readings
float temperature = 0.0;
float humidity = 0.0;

void handleRoot()
{
  String html = "<html><body><h1>ESP32 Web Server</h1>";
  html += "<p><a href=\"/on\"><button>Turn ON LED</button></a></p>";
  html += "<p><a href=\"/off\"><button>Turn OFF LED</button></a></p>";
  html += "<p>Temperature: " + String(temperature) + " °C</p>";
  html += "<p>Humidity: " + String(humidity) + " %</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

int led = 10;

void handleLEDOn()
{
  digitalWrite(led, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
  server.send(200, "text/html", "LED is ON");
}

void handleLEDOff()
{
  digitalWrite(led, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  server.send(200, "text/html", "LED is OFF");
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT); // Cài đặt chân LED là output
  pinMode(led, OUTPUT);
  dht20.begin();
  
  // Kết nối WiFi với IP tĩnh
  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", handleLEDOn);
  server.on("/off", handleLEDOff);

  server.begin();
  Serial.println("HTTP server started");
  xTaskCreate(TaskTemperatureHumidity, "Task Temperature", 2048, NULL, 2, NULL);
}

void loop()
{
  server.handleClient();
}

void TaskTemperatureHumidity(void *pvParameters)
{ // This is a task.
  while (1)
  {
    dht20.read();
    temperature = dht20.getTemperature();
    humidity = dht20.getHumidity();
    delay(5000);
  }
}
