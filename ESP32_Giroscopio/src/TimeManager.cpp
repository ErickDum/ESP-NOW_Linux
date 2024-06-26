#include <Arduino.h>
#include <TimeManager.h>
#include <WiFiManager.h>

// Servidor NTP
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;          // Offset horario en segundos (0 para GMT)
const int daylightOffset_sec = 3600;   // Offset horario de horario de verano en segundos


void initTimestamp(){
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Configuración de hora exitosa");
  delay(1000);
}

// Obtener la hora local
unsigned long getEpochTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Error al obtener la hora local");
    return 0;
  }
  time_t epochTime = mktime(&timeinfo);
  return static_cast<unsigned long>(epochTime);
}

// Obtener el valor del timestamp
void valueTimestamp(mensaje_espnow *mensaje) {
  unsigned long epochTime= getEpochTime();
  if (epochTime > 0) {
    mensaje->timestamp = epochTime;
  }
  Serial.println("Timestamp: " + String(mensaje->timestamp));
}

// Configurar timestamp
void config_timestamp(mensaje_espnow *mensaje){
    initWiFi();
    initTimestamp();
    valueTimestamp(mensaje);

    disconnectWiFi();
}