/* adafruit.ino: this program allows the ESP8266 to send values to io.adafruit.com 
    edit lines 10, 11, 17, and 18 with your credentials 
    switch between moisture and temperature readings with lines 60-74
    */
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/* WiFi Credentials */
#define SSID "WIFI NAME"
#define PASS "WIFI PASSWORD"
// note: ESP8266 does not support 5Gz connections

/* Boker Credentials */
#define SERVER "io.adafruit.com"
#define SERVERPORT 1883 // use 8883 for SSL
#define USERNAME "USER"
#define KEY "KEY"

/* Soil Probe Globals */
#define ADC_PIN 0 // ADC pin on ESP8266
int value;

/* Feeds & Classes */
WiFiClient client;  // use WiFiClientSecure for SSL
Adafruit_MQTT_Client mqtt(&client, SERVER, SERVERPORT, USERNAME, KEY);
Adafruit_MQTT_Publish moisture = Adafruit_MQTT_Publish(&mqtt, USERNAME "/feeds/moisture");
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, USERNAME "/feeds/temperature");
// note: Adafruit uses paths of the form <username>/feeds/<feedname>

void setup() {
  Serial.begin(9600);

  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
}

void loop() {
  mqtt_connect(); // maintains MQTT connection

  // optional: add subscriptions and receiver for messages

  value = analogRead(ADC_PIN) * 1000;
  Serial.print("Sending: ");
  Serial.print(value);
  Serial.println("...");

  // moisture readings
  if (!moisture.publish(value)) {
    Serial.println("Failed");
  } 
  else {
    Serial.println("OK SENT!");
  }

  // temperature readings
  // if (!temperature.publish(value)) {
  //   Serial.println("Failed");
  // } 
  // else {
  //   Serial.println("OK SENT!");
  // }

  delay(5000); // 5 seconds

  // optional: add ping feature
}

/* Tries to connect device five times if it is disconnected */
void mqtt_connect() {
  if (mqtt.connected()) {
    return;
  }

  Serial.println("Connecting to MQTT... ");

  int8_t ret;
  uint8_t retries = 5;
  while ((ret = mqtt.connect()) != 0) { 
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);

       retries--;
       if (retries == 0) {
         Serial.println("Could not connect. Troubleshoot Device");
         while(1){}
       }
  }

  Serial.println("MQTT Connected!");
}