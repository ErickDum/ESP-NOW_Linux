#ifndef ESPNowManager_h
#define ESPNowManager_h

#include <esp_now.h>
#include <App.h>

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void inicializarESPNow(uint8_t CHANNEL, uint8_t broadcastAddress[]);
void enviar_mensaje(mensaje_espnow* mensaje, uint8_t broadcastAddress[]);

#endif