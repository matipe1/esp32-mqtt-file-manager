#ifndef SENSORDHT_H
#define SENSORDHT_H

#include <DHT.h>

class SensorDHT {
private:
    DHT dht;

public:
    // Constructor
    SensorDHT(uint8_t pin, uint8_t tipo);

    // Inicializa el sensor
    void begin();

    // Lecturas
    float leerTemperatura();
    float leerHumedad();
};

#endif