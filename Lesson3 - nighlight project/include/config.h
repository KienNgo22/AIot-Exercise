#pragma once

// WiFi settings
const char* SSID = "Nightlight project wifi";
const char* PASSWORD = "password";

// MQTT settings
const std::string BROKER = "test.mosquitto.org"; // Example: "broker.hivemq.com" or "192.168.1.10"
const std::string CLIENT_NAME = "esp32_light_sensor";
const std::string CLIENT_TELEMETRY_TOPIC = "esp32_device/telemetry";
const std::string SERVER_COMMAND_TOPIC = "esp32_device/commands";