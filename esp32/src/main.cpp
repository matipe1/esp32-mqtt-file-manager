#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include "SensorDHT.h"

// ===== CONFIGURACIONES =====
#define SENSOR_DHT_PIN 5
#define DHT_TYPE DHT22

const char* ssid = "Wokwi-GUEST";       // Red WiFi en Wokwi
const char* password = "";              // Sin contraseña en Wokwi

// Configuración MQTT con mDNS
const char* mqtt_hostname = "mqtt-broker.local"; // Nombre mDNS del broker
const int mqtt_port = 1883;
IPAddress mqtt_server_ip;              // IP resuelta dinámicamente

#define MQTT_TOPIC "th/mediciones"

// ===== OBJETOS =====
SensorDHT sensor(SENSOR_DHT_PIN, DHT22);
WiFiClient espClient; // Canal de red
PubSubClient client(espClient); // Cliente TCP/IP que maneja conexiones de red

// ===== FUNCIONES =====
void conectarWiFi() {
  Serial.print("Conectando a WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" ¡Conectado!");
  
  // Inicializar mDNS
  if (!MDNS.begin("esp32-sensor")) {
    Serial.println("Error al inicializar mDNS");
  } else {
    Serial.println("mDNS iniciado");
  }
}

bool resolverMQTTServer() {
  Serial.print("Resolviendo ");
  Serial.print(mqtt_hostname);
  Serial.print("...");
  
  mqtt_server_ip = MDNS.queryHost(mqtt_hostname);
  
  if (mqtt_server_ip == IPAddress(0, 0, 0, 0)) {
    Serial.println(" Falló");
    // Fallback: intentar con IP fija como respaldo
    mqtt_server_ip.fromString("192.168.100.17");
    Serial.print("Usando IP fallback: ");
    Serial.println(mqtt_server_ip);
    return false;
  } else {
    Serial.print(" Resuelto: ");
    Serial.println(mqtt_server_ip);
    return true;
  }
}

void conectarMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println(" ¡Conectado!");
    } else {
      Serial.print(" Falló, rc=");
      Serial.print(client.state());
      Serial.println(" Reintentando en 2s");
      delay(2000);
    }
  }
}

// ===== SETUP =====
void setup() {
  Serial.begin(9600);
  sensor.begin();

  conectarWiFi();
  
  // Resolver la dirección del servidor MQTT
  resolverMQTTServer();
  client.setServer(mqtt_server_ip, mqtt_port);
  
  Serial.println("-- Sistema iniciado --");
}

// ===== LOOP =====
void loop() {
  if (!client.connected()) {
    conectarMQTT();
  }
  client.loop();

  float temp = sensor.leerTemperatura();
  float hum = sensor.leerHumedad();

  // Crear JSON - objeto en memoria que la librería ArduinoJson puede manipular.
  StaticJsonDocument<200> doc; // documento JSON en memoria con capacidad de 200 bytes.
  doc["device"] = "esp32-01";
  doc["temperature"] = temp;
  doc["humidity"] = hum;
  doc["ts"] = millis();  // timestamp en ms desde inicio

  // MQTT requiere que el mensaje sea un string, no un objeto en memoria
  char buffer[128]; // se declara el JSON como arreglo de caracteres
  serializeJson(doc, buffer); // convierte objeto doc en una cadena JSON y lo guarda en el buffer

  // Publicar en MQTT
  if (client.publish(MQTT_TOPIC, buffer)) {
    Serial.println("Mensaje publicado:");
    Serial.println(buffer);
  } else {
    Serial.println("Error al publicar mensaje");
  }

  delay(5000); // Esperar 5 segundos antes de la próxima lectura
}