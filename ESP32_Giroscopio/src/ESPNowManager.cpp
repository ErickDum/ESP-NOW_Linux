#include <Arduino.h>
#include <ESPNowManager.h>
#include <WiFi.h>
#include <esp_wifi.h>

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Mensaje enviado con estado: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Éxito" : "Fallo");
}

void inicializarESPNow(uint8_t CHANNEL, uint8_t broadcastAddress[]) {
    // Inicializar WiFi en modo STA y establecer el canal
    WiFi.mode(WIFI_STA);
    esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N);
    esp_wifi_config_espnow_rate(WIFI_IF_STA, WIFI_PHY_RATE_54M);
    
    // Inicializar ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error al inicializar ESP-NOW");
        return;
    }
    
    // Registrar callback de envío de datos
    esp_now_register_send_cb(OnDataSent);
    
    // Añadir la dirección de broadcast como peer
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(esp_now_peer_info_t));
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = CHANNEL;  
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Error al añadir el peer");
        return;
    }
}

void enviar_mensaje(mensaje_espnow* mensaje, uint8_t broadcastAddress[]) {

    uint64_t msg = 0;

    msg |= static_cast<int16_t>(mensaje->data);
    msg  = (msg << 16) | static_cast<uint32_t>(mensaje->contador);
    msg  = (msg << 32) | static_cast<uint32_t>(mensaje->timestamp);
    

    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &msg, sizeof(msg));
    if (result != ESP_OK) {
        Serial.println("Error al enviar el mensaje");
    } else {
        Serial.println("Mensaje enviado");
    }

    Serial.println(msg);
    Serial.println(mensaje->timestamp);
    Serial.println(mensaje->data);
    Serial.println(mensaje->contador);
}
