#include <stdint.h>
#include <stdio.h>

#include <assert.h>
#include <unistd.h>
#include <sys/time.h>

#include <thread>

#include "ESPNOW_manager.h"
#include "ESPNOW_types.h"

using namespace std;

static uint8_t my_mac[6] = {0xA4, 0xC3, 0xF0, 0xE2, 0x75, 0xE8};
static uint8_t dest_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t ESP_mac[6] = {0x34, 0x86, 0x5D,0x3B, 0x45, 0xD8};

ESPNOW_manager *handler;

uint8_t payload[127];


void callback(uint8_t src_mac[6], uint8_t *data, int len) {
	handler->mypacket.wlan.actionframe.content.length = 127 + 5;
	memcpy(handler->mypacket.wlan.actionframe.content.payload, data, 6);
	//handler->set_dst_mac(dest_mac);
	handler->send();
	printf(" MAC: ");
	FILE *archivo = fopen("datos.csv","a");
	for (int i = 0; i < 6; i++){
        printf("%02x-", src_mac[i]);
    }
	
	for (int i = 0; i < len; i+=8){	
		uint32_t timestamp = data[i+3] <<24 |data[i+2] << 16 |data[i+1] << 8 | data[i];
		uint16_t contador = data[i+5] <<8 |data[i+4];
		int16_t giroscopio = data[i+7] <<8 |data[i+6];
		printf(" Timestamp: ");
        printf("%d", timestamp);
		printf(" Contador: ");
		printf("%d", contador);
		printf(" Data: ");
		printf("%d", giroscopio);
		fprintf(archivo, "%d, %d, %d", timestamp, contador, giroscopio);
	}
	printf("\n");
	fprintf(archivo, "\n");
	fclose(archivo);
}

int main(int argc, char **argv) {

	assert(argc > 1);

	nice(-20);

	handler = new ESPNOW_manager(argv[1], DATARATE_54Mbps, CHANNEL_freq_1, dest_mac, my_mac, false);

	handler->set_filter(ESP_mac, dest_mac);

	handler->set_recv_callback(&callback);

	handler->start();

	while(1) {
		std::this_thread::yield();
	}
	

	handler->end();
	
}
