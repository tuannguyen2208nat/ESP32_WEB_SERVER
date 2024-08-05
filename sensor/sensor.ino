#include <Wire.h>
#include <DHT20.h>
#include "colors.h"
#include <Ultrasonic.h>
#include <ESP32Servo.h>
#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>


void TaskLCD(void *pvParameters);
void TaskMFM(void *pvParameters); //Mini fan motor
void TaskSMS(void *pvParameters); //Soil moisture 
void TaskPIR(void *pvParameters); //Passive infrared sensor
void TaskUSM(void *pvParameters); //Ultrasonic sensor module
void Task2CSM(void *pvParameters); //2-Channel switching module
void TaskServo(void *pvParameters);
void TaskRelay(void *pvParameters);
void TaskLight(void *pvParameters);
void TaskLedRGB(void *pvParameters);
void TaskTemperatureHumidity(void *pvParameters);


void setup() {
  Serial.begin(115200);
  Wire.begin();

  xTaskCreate(TaskLCD, "TaskLCD", 2048, NULL, 2, NULL);
  xTaskCreate(TaskMFM, "TaskMFM", 2048, NULL, 2, NULL);
  xTaskCreate(TaskSMS, "TaskSMS", 2048, NULL, 2, NULL);
  xTaskCreate(TaskPIR, "TaskPir", 2048, NULL, 2, NULL);
  xTaskCreate(TaskUSM, "TaskUSM", 2048, NULL, 2, NULL);
  xTaskCreate(Task2CSM, "Task2CSM", 2048, NULL, 2, NULL);
  xTaskCreate(TaskServo, "TaskServo", 2048, NULL, 2, NULL);
  xTaskCreate(TaskRelay, "TaskRelay", 2048, NULL, 2, NULL);
  xTaskCreate(TaskLight, "TaskLight", 2048, NULL, 2, NULL);
  xTaskCreate(TaskLedRGB, "TaskLedRGB", 2048, NULL, 2, NULL);
  xTaskCreate(TaskTemperatureHumidity, "TaskTemperatureHumidity", 2048, NULL, 2, NULL);

  Serial.println("Start");
}

void loop() {
  Serial.println();
  delay(5000);
}

void TaskLCD(void *pvParameters) {
  LiquidCrystal_I2C lcd(33,16,2);
  lcd.begin();

  while (1) {
  lcd.setCursor((16 - 5) / 2, 0);
  lcd.print("Hello");  
  lcd.setCursor(0,1);
  lcd.print("My name is Tuan");  
  vTaskDelay(5000 / portTICK_PERIOD_MS); 
  lcd.clear();
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

void TaskSMS(void *pvParameters) {
  int sensorPin = D3;

  while (1) {
  Serial.println("Soil moisture : " + String(analogRead(sensorPin)) +" %");
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

void TaskUSM(void *pvParameters) {
  int sensorPin_Left = D9; 
  int sensorPin_Right = D10; 
  Ultrasonic ultrasonic(sensorPin_Left, sensorPin_Right);

  while (1) {
  Serial.println("Distance : " + String(ultrasonic.read()) + " cm");
  vTaskDelay(5000 / portTICK_PERIOD_MS); 
  }
}

void Task2CSM(void *pvParameters) {
  int sensorPin = D5; 
  pinMode(sensorPin, OUTPUT);
  bool check = true;

  while (1) {
  digitalWrite(sensorPin, check ? HIGH : LOW);
  Serial.println("2CSM : " + String(check ? "ON" : "OFF"));
  check = !check;
  vTaskDelay(5000 / portTICK_PERIOD_MS); 
  }
}

void TaskServo(void *pvParameters) {
  int sensorPin = D2; 
  int pos = 0;
  Servo myservo;
  myservo.attach(sensorPin);
  myservo.write(90);
  bool check = true;

  while (1) {
  if(check)
  {
    for (pos=0; pos<=180; pos++) { 
    myservo.write(pos);
    }
  }
  else{
  for (pos=180; pos>=0; pos--) {
    myservo.write(pos);
  }
  }
  check=!check;
  vTaskDelay(5000 / portTICK_PERIOD_MS); 
  }
}

void TaskRelay(void *pvParameters) {
  int sensorPin = D3;
  pinMode(sensorPin, OUTPUT); 
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

void TaskLedRGB(void *pvParameters) {
  int sensorPin = D7; 
  Adafruit_NeoPixel rgb(4, sensorPin, NEO_GRB + NEO_KHZ800);
  uint32_t colors[] = {RED, ORANGE, YELLOW, GREEN, BLUE, INDIGO, PURPLE, WHITE, BLACK, OFF};
  const char* colorNames[] = {"RED", "ORANGE", "YELLOW", "GREEN", "BLUE", "INDIGO", "PURPLE", "WHITE", "BLACK", "OFF"};
  int numColors = sizeof(colors) / sizeof(colors[0]);
  rgb.begin();

  while (1) {
  for (int i = 0; i < numColors; i++) {
  for (int j = 0; j < 4; j++) {
  rgb.setPixelColor(j, colors[i]);
  }
  rgb.show();
  Serial.println("Led RGB : " + String(colorNames[i]));
  vTaskDelay(5000 / portTICK_PERIOD_MS); 
  }
  }
}

void TaskTemperatureHumidity(void *pvParameters) {
  DHT20 dht20;
  dht20.begin();

  while (1) {
    dht20.read();
    Serial.println("Temperature : " + String(dht20.getTemperature())+ " - " + "Humidity : " + String(dht20.getHumidity()));
    vTaskDelay(5000 / portTICK_PERIOD_MS); 
  }
}






