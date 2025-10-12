#include <Arduino.h>
#include "SensorDHT.h"

#define SENSOR_DHT_PIN 5

SensorDHT sensor(SENSOR_DHT_PIN, DHT22);

void setup() {
  Serial.begin(9600);

  sensor.begin();
  Serial.println("Sistema iniciado");
}

void loop() {
  float temp = sensor.leerTemperatura();
  float hum = sensor.leerHumedad();

  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.print(" °C  |  Humedad: ");
  Serial.print(hum);
  Serial.println(" %");

  delay(2000); // Esperar 2 segundos antes de la próxima lectura
}