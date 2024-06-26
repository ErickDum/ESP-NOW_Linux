#include <Arduino.h>
#include <App.h>
#include <ESPNowManager.h>
#include <TimeManager.h>
#include <MPU6050Manager.h>

#define CHANNEL 1

// Dirección MAC de broadcast
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

mensaje_espnow mensaje;

void init_message(mensaje_espnow *mensaje);
void update_message(mensaje_espnow *mensaje);

void setup() {
    Serial.begin(115200);

    config_timestamp(&mensaje);
    inicializarESPNow(CHANNEL, broadcastAddress);
    initMPU();
    init_message(&mensaje);
}

void loop() {
    int16_t init_time = millis();
    update_message(&mensaje);
    update_gyroscope(&mensaje);
    enviar_mensaje(&mensaje, broadcastAddress);

    int16_t end_time = millis();
    delay(250 - (end_time - init_time));
}


void init_message(mensaje_espnow *mensaje){
    mensaje->contador = 0;
    mensaje->data = 0;
}

void update_message(mensaje_espnow *mensaje){
    mensaje->contador++;
    mensaje->timestamp++;
}