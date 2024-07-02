#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>
#include <thread>
#include <vector>

#include "ESPNOW_manager.h"
#include "ESPNOW_types.h"

#include <iostream>
#include <bitset>
#include <cstdio>
#include <cstdint>
#include <inttypes.h>
#include <cstdlib>

using namespace std;

static uint8_t my_mac[6] = {0x00, 0x0F, 0x60, 0x0A, 0x28, 0x3F};
//static uint8_t my_mac[6] = {0x20, 0xE0, 0x17, 0x0F, 0x0C, 0xB8};

vector<array<uint8_t, 6>> esp_macs = {
	{0x78, 0xE3, 0x6D, 0x19, 0xE5, 0x28},
	{0x94, 0xB5, 0x55, 0x25, 0x7C, 0x24},
	{0x0C, 0xB8, 0x15, 0xC2, 0x42, 0x2C},
	{0x34, 0x86, 0x5D, 0x3B, 0x45, 0xD8},
	{0x94, 0xB5, 0x55, 0x2C, 0x81, 0x48},
};

ESPNOW_manager *handler;

uint8_t payload[127];
char buffer[32];

void callback(uint8_t src_mac[6], uint8_t *data, int len) {
	FILE *archivo = fopen("Packages_ESPNOW_S21_Nodos_4","a");
	FILE* pipe = popen("date -u +%s%6N | tr -d '\n'", "r");
	fgets(buffer, sizeof(buffer), pipe);

	handler->mypacket.wlan.actionframe.content.length = 127 + 5;
	memcpy(handler->mypacket.wlan.actionframe.content.payload, data, 6);
	handler->send();
	for (int i = 0; i < 6; i++)
    {
        printf("%02x-", src_mac[i]);
		fprintf(archivo, "%02x- ,", src_mac[i]);
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
	fprintf(archivo,"\n");
	fclose(archivo);
	pclose(pipe);
}

void start_handler(const array<uint8_t, 6>& esp_mac) {
    uint8_t dest_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    handler = new ESPNOW_manager("wlan1", DATARATE_54Mbps, CHANNEL_freq_1, dest_mac, my_mac, false); 
    handler->set_filter(const_cast<uint8_t*>(esp_mac.data()), dest_mac);
    handler->set_recv_callback(&callback);
    handler->start();
    while (1) {
        std::this_thread::yield();
    }
    delete handler;
}

int main(int argc, char **argv) {
	assert(argc > 1);
	nice(-20);
	vector<std::thread> threads;
	    for (const auto& esp_mac : esp_macs) {
        threads.emplace_back(start_handler, std::ref(esp_mac));
    }

    bool allThreadsRunning = true;

    while (allThreadsRunning) {
        allThreadsRunning = false;
        for (auto& t : threads) {
            if (t.joinable()) {
                allThreadsRunning = true;
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
