#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#define CHANNEL 1 // Especifica el canal que deseas usar

// Dirección MAC de broadcast
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Estructura del mensaje
typedef struct {
    int contador;
} mensaje_espnow;

mensaje_espnow mensaje;

// Callback cuando el mensaje es enviado
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Mensaje enviado con estado: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Éxito" : "Fallo");
}

void setup() {
    // Inicializar Serial Monitor
    Serial.begin(115200);
    
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

    // Inicializar contador
    mensaje.contador = 0;
}

void loop() {
    // Incrementar el contador
    mensaje.contador++;
    String msg = "mensaje" + String(mensaje.contador);
    
    // Enviar mensaje
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &msg, sizeof(msg));

    if (result == ESP_OK) {
        Serial.println("Mensaje enviado con éxito");
    } else {
        Serial.println("Error al enviar el mensaje");
    }
    Serial.println(msg);

    // Esperar 1 segundo antes de enviar el siguiente mensaje
    delay(1000);
}
