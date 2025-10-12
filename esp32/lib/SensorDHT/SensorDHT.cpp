#include "SensorDHT.h"

// Constructor
SensorDHT::SensorDHT(uint8_t pin, uint8_t tipo) : dht(pin, tipo) {}

// Inicializa el sensor
void SensorDHT::begin() {
    dht.begin();
}

// Devuelve la temperatura
float SensorDHT::leerTemperatura() {
    return dht.readTemperature();
}

// Devuelve la humedad
float SensorDHT::leerHumedad() {
    return dht.readHumidity();
}