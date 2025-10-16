// ESP32 con Auto-Discovery de broker MQTT
// Esta versión busca automáticamente el broker en la red local
// filepath: /home/estudiante/TA/esp32-mqtt-file-manager/esp32/src/main_dynamic.cpp

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

// Configuración MQTT dinámica
String mqtt_server_ip = "";
const int mqtt_port = 1883;

#define MQTT_TOPIC "th/mediciones"

// ===== OBJETOS =====
SensorDHT sensor(SENSOR_DHT_PIN, DHT22);
WiFiClient espClient;
PubSubClient client(espClient);

// ===== FUNCIONES =====
void conectarWiFi() {
  Serial.print("Conectando a WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" ¡Conectado!");
  Serial.print("IP ESP32: ");
  Serial.println(WiFi.localIP());
}

String detectarBrokerMQTT() {
  // Obtener IP local del ESP32
  IPAddress localIP = WiFi.localIP();
  String networkBase = String(localIP[0]) + "." + String(localIP[1]) + "." + String(localIP[2]) + ".";
  
  Serial.println("🔍 Buscando broker MQTT en la red...");
  
  // Lista de IPs comunes para el broker
  String candidatos[] = {
    networkBase + "1",    // Router/Gateway
    networkBase + "100",  // IP común para PCs
    networkBase + "101",
    networkBase + "17",   // Tu IP actual
    "192.168.1.100",     // Redes comunes
    "192.168.0.100",
    "10.0.0.100"
  };
  
  WiFiClient testClient;
  
  for (String ip : candidatos) {
    Serial.print("Probando: ");
    Serial.print(ip);
    Serial.print(":1883... ");
    
    if (testClient.connect(ip.c_str(), 1883)) {
      testClient.stop();
      Serial.println("✅ ¡Encontrado!");
      return ip;
    } else {
      Serial.println("❌");
    }
    delay(100);
  }
  
  // Si no encuentra ninguno, usar IP por defecto
  Serial.println("⚠️ No se encontró broker, usando IP por defecto");
  return networkBase + "1";
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
  
  // Detectar automáticamente el broker MQTT
  mqtt_server_ip = detectarBrokerMQTT();
  client.setServer(mqtt_server_ip.c_str(), mqtt_port);
  
  Serial.print("🎯 Broker MQTT configurado: ");
  Serial.println(mqtt_server_ip);
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

  // Crear JSON
  StaticJsonDocument<200> doc;
  doc["device"] = "esp32-01";
  doc["temperature"] = temp;
  doc["humidity"] = hum;
  doc["ts"] = millis();
  doc["broker_ip"] = mqtt_server_ip; // Incluir IP del broker para debug

  char buffer[128];
  serializeJson(doc, buffer);

  // Publicar en MQTT
  if (client.publish(MQTT_TOPIC, buffer)) {
    Serial.println("Mensaje publicado:");
    Serial.println(buffer);
  } else {
    Serial.println("Error al publicar mensaje");
  }

  delay(5000);
}
