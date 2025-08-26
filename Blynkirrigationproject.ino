#define BLYNK_TEMPLATE_ID "TMPL3TWmI0bn9"
#define BLYNK_TEMPLATE_NAME "Smart Irrigation"
#define BLYNK_AUTH_TOKEN "5xGHdxiZult31E7X9o2fo7jlpJhEvk2N"

#define BLYNK_PRINT Serial  // <— enables Blynk debug logs to Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x27

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char ssid[] = "vivo T1 5G";
char pass[] = "12345678";

int soilPin = 34;
int rainPin = 35;
int soilValue = 0;
int rainValue = 0;
#define DHTPIN 4
#define DHTTYPE DHT22
#define PIR_PIN 26
#define RELAY_PIN 27



DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

float t = 0;
float h = 0;
int pirValue = 0;

void sendSensorData() {
  soilValue = analogRead(soilPin);
  rainValue = analogRead(rainPin);
  Serial.print("Soil Moisture Value: ");
  Serial.println(soilValue);
  Serial.print("Rain sensor Value: ");
  Serial.println(rainValue);
  Blynk.virtualWrite(V2, soilValue);
  Blynk.virtualWrite(V1, rainValue);
  t= dht.readTemperature();
  h= dht.readHumidity();
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V7, h);
  // OLED update without flicker
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("T:"); display.print(t); display.print("C  ");
  display.print("H:"); display.print(h); display.print("%  ");

  if (pirValue == HIGH) {
    display.setCursor(0,16);
    display.println("Motion Detected   ");
  } else {
    display.setCursor(0,16);
    display.print("Soil:"); display.print(soilValue);
    display.print(" Rain:"); display.print(rainValue);
    display.println("   ");
  }
  display.display();
}

void setup() {
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println("Smart Irrigation");
  display.display();
  delay(2000);

  // Initialize sensors
  dht.begin();
   
  Serial.println("Connecting to Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
  pirValue = digitalRead(PIR_PIN);
  if (pirValue == HIGH){
    Serial.println("Motion Detected");
    digitalWrite(RELAY_PIN, LOW);
    Blynk.virtualWrite(V5, 1);
  }else{
    Serial.println("No Motion");
    digitalWrite(RELAY_PIN, HIGH);
    Blynk.virtualWrite(V5, 0);
  timer.setInterval(2000L, sendSensorData);
  }
 
}