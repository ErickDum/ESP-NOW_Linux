#include <Arduino.h>
#include <Wire.h>
#include <MPU6050Manager.h>

// Dirección del MPU6050
const int MPU6050_ADDRESS = 0x68;

// Registros del MPU6050
const int PWR_MGMT_1 = 0x6B;
const int GYRO_ZOUT_H = 0x47;

// Factores de conversión para el giroscopio
const float GYRO_SCALE_FACTOR = 131.0;

// Variables de compensación
float gyro_offset_z = 0;

// Variables para el cálculo de la orientación
float gyro_z_angle = 0.0;

// Tiempo de muestreo (en segundos)
const float dt = 0.25;  // Ajusta esto según tu frecuencia de muestreo


// inicializar MPU6050
void initMPU(){
  Wire.begin();
  // Inicializar el MPU6050
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(PWR_MGMT_1);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.println("MPU6050 Configurado");
  
}

// Función para leer valores de 16 bits a partir de dos registros consecutivos
int readWord(int reg) {
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(static_cast<uint8_t>(MPU6050_ADDRESS), static_cast<size_t>(2), true);
  int high = Wire.read();
  int low = Wire.read();
  int value = (high << 8) | low;
  if (value >= 0x8000) {
    return -((65535 - value) + 1);
  } else {
    return value;
  }
}

void update_gyroscope(mensaje_espnow *mensaje){
  float gyro_z = float((readWord(GYRO_ZOUT_H) - gyro_offset_z) / GYRO_SCALE_FACTOR);
  gyro_z_angle += gyro_z * dt;
  // Enviar los datos a JETSON NANO
  mensaje->data = gyro_z_angle;
}