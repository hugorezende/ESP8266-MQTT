// This example uses an ESP8266 Board
// to connect to MQTT and send BMP180 data
// BMP180 wiring:
// SCL -> D1
// SDA -> D2
//
// MQTT library: https://github.com/256dpi/arduino-mqtt
// Adafruit_BMP085: https://github.com/adafruit/Adafruit-BMP085-Library
// by Hugo Rezende

#include <Adafruit_BMP085.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>

#ifndef STASSID
#define STASSID "yourWifiSSID"
#define STAPSK  "yourWifiPassword"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;
WiFiClient net;
MQTTClient client;
Adafruit_BMP085 bmp;

unsigned long lastMillis = 0;
uint8_t LED_Pin = D4;

void connect() {
  Serial.print("Checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //Blink the led until wifi connects
    digitalWrite(LED_Pin, HIGH); // Turn the LED on
    delay(50);                
    digitalWrite(LED_Pin, LOW);// Turn the LED off
    delay(50); 
    delay(100);
  }
  Serial.print("\nConnecting...");
  while (!client.connect("arduino", "try", "try")) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected!");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}

void setup() {
  pinMode(LED_Pin, OUTPUT); 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  // Connecting to the MQTT server
  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported by Arduino.
  // You need to set the IP address directly.
  client.begin("192.168.0.10", 1883 ,net); // here you insert the IP/domain of the MQTT server and the port
  connect();
}

void loop() {
  client.loop();
  delay(10);  // fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    // sending values to topics in a JSON format
    client.publish("sensor/temperatura", "{\"value\": "+ String(bmp.readTemperature()) +"}" );
    client.publish("sensor/pressao", "{\"value\": "+ String(bmp.readSealevelPressure()) +"}" );
  }
}
