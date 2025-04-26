#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"

// Pin Definitions
#define LED_PIN 23           // GPIO23 for LED
#define SENSOR_PIN 34        // GPIO34 for light sensor

// WiFi and MQTT Setup
WiFiClient espClient;
PubSubClient client(espClient);

// Timing
unsigned long lastTelemetryTime = 0;
const long telemetryInterval = 2000; // 2 seconds

// Connect to WiFi
void connectWiFi() {
    Serial.println("Connecting to WiFi...");
    WiFi.disconnect(true);
    delay(1000);
    WiFi.begin(SSID, PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        Serial.print(".");
        delay(1000);
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi!");
    } else {
        Serial.println("\nFailed to connect to WiFi.");
    }
}

// MQTT Callback Function
void clientCallback(char *topic, byte *payload, unsigned int length) {
    char buff[length + 1];
    for (unsigned int i = 0; i < length; i++) {
        buff[i] = (char)payload[i];
    }
    buff[length] = '\0';

    Serial.print("Message received: ");
    Serial.println(buff);

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, buff);

    if (!error) {
        JsonObject obj = doc.as<JsonObject>();
        bool led_on = obj["led_on"];
        digitalWrite(LED_PIN, led_on ? HIGH : LOW);
    } else {
        Serial.println("Failed to parse JSON");
    }
}

// Reconnect to MQTT Broker
void reconnectMQTTClient() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(CLIENT_NAME.c_str())) {
            Serial.println("connected");
            client.subscribe(SERVER_COMMAND_TOPIC.c_str());
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" retrying in 5 seconds");
            delay(5000);
        }
    }
}

// Setup MQTT Client
void createMQTTClient() {
    client.setServer(BROKER.c_str(), 1883);
    client.setCallback(clientCallback);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(SENSOR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    connectWiFi();
    createMQTTClient();
}

void loop() {
    if (!client.connected()) {
        reconnectMQTTClient();
    }
    client.loop();

    int light = analogRead(SENSOR_PIN); // 0-4095 for ESP32 ADC
    Serial.print("Light value: ");
    Serial.println(light);

    // Local control of LED based on light level
    if (light < 1200) {
        digitalWrite(LED_PIN, HIGH); // Turn ON LED
    } else {
        digitalWrite(LED_PIN, LOW); // Turn OFF LED
    }

    // Send telemetry to MQTT every 2 seconds
    if (millis() - lastTelemetryTime >= telemetryInterval) {
        lastTelemetryTime = millis();

        DynamicJsonDocument doc(1024);
        doc["light"] = light;

        String telemetry;
        serializeJson(doc, telemetry);

        Serial.print("Sending telemetry: ");
        Serial.println(telemetry);

        client.publish(CLIENT_TELEMETRY_TOPIC.c_str(), telemetry.c_str());
    }

    delay(1000); // Delay for sensor reading refresh
}
