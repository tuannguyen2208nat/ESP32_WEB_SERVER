#include <Wire.h>
#include <DHT20.h>

DHT20 dht20;

void TaskTemperatureHumidity(void *pvParameters);
void TaskRelay(void *pvParameters);
void TaskLight(void *pvParameters);
void TaskPIR(void *pvParameters); //Passive infrared sensor
void TaskMFM(void *pvParameters); //Mini fan motor


void setup() {
  Serial.begin(115200);

  xTaskCreate(TaskTemperatureHumidity, "TaskTemperatureHumidity", 2048, NULL, 2, NULL);
  xTaskCreate(TaskRelay, "TaskRelay", 2048, NULL, 2, NULL);
  xTaskCreate(TaskLight, "TaskLight", 2048, NULL, 2, NULL);
  xTaskCreate(TaskPIR, "TaskPir", 2048, NULL, 2, NULL);
  xTaskCreate(TaskMFM, "TaskMFM", 2048, NULL, 2, NULL);

  Serial.println("Start");
}

void loop() {
  Serial.println();
  delay(5000);

}

void TaskTemperatureHumidity(void *pvParameters) {
  dht20.begin();

  while (1) {
    dht20.read();
    Serial.println("Temperature : " + String(dht20.getTemperature())+ " - " + "Humidity : " + String(dht20.getHumidity()));
    vTaskDelay(5000 / portTICK_PERIOD_MS); 
  }
}

void TaskRelay(void *pvParameters) {
  int sensorPin = D3;
  pinMode(D3, OUTPUT); 
  bool check = true;

  while (1) {
    digitalWrite(sensorPin, check ? HIGH : LOW); 
    Serial.println("Relay : " + String(check ? "ON" : "OFF"));
    check = !check;  
    vTaskDelay(5000 / portTICK_PERIOD_MS); 
  }
}

void TaskLight(void *pvParameters) {
  int sensorPin = D5;

  while (1) {
  Serial.println("Light : " + String(analogRead(sensorPin)) +" lux");
  vTaskDelay(5000 / portTICK_PERIOD_MS); 
  }
}

void TaskPIR(void *pvParameters) {
  int sensorPin = D7;
  pinMode(sensorPin, INPUT);

  while (1) {
  Serial.println("PIR : " + String(analogRead(sensorPin)?"detect":"not detect"));
  vTaskDelay(5000 / portTICK_PERIOD_MS); 
  }
}

void TaskMFM(void *pvParameters) {
  int sensorPin = D9;
  pinMode(sensorPin, OUTPUT);
  bool check = true;

  while (1) {
  digitalWrite(sensorPin, check ? HIGH : LOW); 
  Serial.println("MFM : " + String(check ? "ON" : "OFF"));
  check = !check;  
  vTaskDelay(5000 / portTICK_PERIOD_MS); 
  }
}

