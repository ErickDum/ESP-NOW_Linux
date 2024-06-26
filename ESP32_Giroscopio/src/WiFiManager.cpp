#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

// Credenciales de la red WiFi
const char* ssid       = "Redmi";
const char* password   = "geova030297";

void initWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a la red Wi-Fi...");
  }
  Serial.println("Conexión exitosa a la red Wi-Fi");
}

void disconnectWiFi() {
  WiFi.disconnect();
}