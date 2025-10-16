// Versión con WiFiManager y configuración web
// filepath: /home/estudiante/TA/esp32-mqtt-file-manager/esp32/src/main_wifimanager.cpp

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <Preferences.h>
#include "SensorDHT.h"

// ===== CONFIGURACIONES =====
#define SENSOR_DHT_PIN 5
#define DHT_TYPE DHT22
#define MQTT_TOPIC "th/mediciones"

// Variables globales para configuración
String mqtt_server_ip = "";
int mqtt_port = 1883;
String device_name = "esp32-01";

// ===== OBJETOS =====
SensorDHT sensor(SENSOR_DHT_PIN, DHT22);
WiFiClient espClient;
PubSubClient client(espClient);
WebServer server(80);
Preferences preferences;

// ===== FUNCIONES WEB =====
void handleRoot() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 MQTT Config</title>
    <meta charset='utf-8'>
    <style>
        body { font-family: Arial; margin: 40px; background: #f0f0f0; }
        .container { background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        input { width: 100%; padding: 8px; margin: 5px 0; border: 1px solid #ddd; border-radius: 4px; }
        button { background: #007bff; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; }
        button:hover { background: #0056b3; }
        .status { padding: 10px; margin: 10px 0; border-radius: 4px; }
        .success { background: #d4edda; color: #155724; border: 1px solid #c3e6cb; }
        .info { background: #d1ecf1; color: #0c5460; border: 1px solid #bee5eb; }
    </style>
</head>
<body>
    <div class='container'>
        <h2>🔧 Configuración ESP32 MQTT</h2>
        
        <div class='status info'>
            <strong>Estado actual:</strong><br>
            WiFi: )" + WiFi.SSID() + R"( ()" + WiFi.localIP().toString() + R"()<br>
            MQTT: )" + mqtt_server_ip + R"(:)" + String(mqtt_port) + R"(<br>
            Dispositivo: )" + device_name + R"(
        </div>

        <form action='/save' method='POST'>
            <h3>⚙️ Configuración MQTT</h3>
            <label>IP del Broker MQTT:</label>
            <input type='text' name='mqtt_ip' value=')" + mqtt_server_ip + R"(' placeholder='192.168.1.100'>
            
            <label>Puerto MQTT:</label>
            <input type='number' name='mqtt_port' value=')" + String(mqtt_port) + R"(' placeholder='1883'>
            
            <label>Nombre del dispositivo:</label>
            <input type='text' name='device_name' value=')" + device_name + R"(' placeholder='esp32-01'>
            
            <br><br>
            <button type='submit'>💾 Guardar Configuración</button>
        </form>
        
        <br>
        <button onclick='location.href="/scan"'>🔍 Buscar Broker Automáticamente</button>
        <button onclick='location.href="/restart"'>🔄 Reiniciar ESP32</button>
    </div>
</body>
</html>
  )";
  server.send(200, "text/html", html);
}

void handleSave() {
  if (server.hasArg("mqtt_ip")) {
    mqtt_server_ip = server.arg("mqtt_ip");
    preferences.putString("mqtt_ip", mqtt_server_ip);
  }
  if (server.hasArg("mqtt_port")) {
    mqtt_port = server.arg("mqtt_port").toInt();
    preferences.putInt("mqtt_port", mqtt_port);
  }
  if (server.hasArg("device_name")) {
    device_name = server.arg("device_name");
    preferences.putString("device_name", device_name);
  }
  
  // Reconfigurar cliente MQTT
  client.setServer(mqtt_server_ip.c_str(), mqtt_port);
  
  server.send(200, "text/html", 
    "<html><body><h2>✅ Configuración guardada!</h2>"
    "<p>Redirigiendo...</p>"
    "<script>setTimeout(function(){window.location='/';}, 2000);</script>"
    "</body></html>");
}

void handleScan() {
  String result = buscarBrokerAutomatico();
  server.send(200, "text/html", 
    "<html><body><h2>🔍 Búsqueda automática</h2>"
    "<p>" + result + "</p>"
    "<a href='/'>← Volver</a></body></html>");
}

void handleRestart() {
  server.send(200, "text/html", 
    "<html><body><h2>🔄 Reiniciando...</h2></body></html>");
  delay(1000);
  ESP.restart();
}

// ===== FUNCIONES MQTT =====
String buscarBrokerAutomatico() {
  IPAddress localIP = WiFi.localIP();
  String networkBase = String(localIP[0]) + "." + String(localIP[1]) + "." + String(localIP[2]) + ".";
  
  String candidatos[] = {
    networkBase + "1",
    networkBase + "17",
    networkBase + "100",
    networkBase + "101",
    "192.168.1.100",
    "192.168.0.100"
  };
  
  WiFiClient testClient;
  
  for (String ip : candidatos) {
    if (testClient.connect(ip.c_str(), 1883)) {
      testClient.stop();
      mqtt_server_ip = ip;
      preferences.putString("mqtt_ip", mqtt_server_ip);
      client.setServer(mqtt_server_ip.c_str(), mqtt_port);
      return "✅ Broker encontrado en: " + ip;
    }
    delay(50);
  }
  
  return "❌ No se encontró ningún broker MQTT activo";
}

void conectarMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT...");
    if (client.connect(device_name.c_str())) {
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
  Serial.begin(115200);
  sensor.begin();
  
  // Inicializar preferencias
  preferences.begin("mqtt-config", false);
  mqtt_server_ip = preferences.getString("mqtt_ip", "");
  mqtt_port = preferences.getInt("mqtt_port", 1883);
  device_name = preferences.getString("device_name", "esp32-01");

  // WiFiManager para configuración automática de WiFi
  WiFiManager wm;
  
  // Descomentar para reset de configuración en desarrollo
  // wm.resetSettings();
  
  // Intentar conectar automáticamente, si falla abre AP
  if (!wm.autoConnect("ESP32-MQTT-Config")) {
    Serial.println("Error conectando WiFi");
    delay(3000);
    ESP.restart();
  }

  Serial.println("WiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Configurar mDNS
  if (MDNS.begin("esp32-mqtt")) {
    Serial.println("mDNS iniciado: http://esp32-mqtt.local");
  }

  // Si no hay IP configurada, buscar automáticamente
  if (mqtt_server_ip == "") {
    Serial.println("No hay broker configurado, buscando automáticamente...");
    buscarBrokerAutomatico();
  }

  // Configurar servidor web
  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/scan", handleScan);
  server.on("/restart", handleRestart);
  server.begin();

  // Configurar cliente MQTT
  if (mqtt_server_ip != "") {
    client.setServer(mqtt_server_ip.c_str(), mqtt_port);
    Serial.println("MQTT configurado: " + mqtt_server_ip + ":" + String(mqtt_port));
  }

  Serial.println("=== Sistema iniciado ===");
  Serial.println("Configuración web: http://" + WiFi.localIP().toString());
}

// ===== LOOP =====
void loop() {
  server.handleClient();
  
  if (mqtt_server_ip != "" && !client.connected()) {
    conectarMQTT();
  }
  
  if (client.connected()) {
    client.loop();

    static unsigned long lastSend = 0;
    if (millis() - lastSend > 5000) {
      float temp = sensor.leerTemperatura();
      float hum = sensor.leerHumedad();

      StaticJsonDocument<200> doc;
      doc["device"] = device_name;
      doc["temperature"] = temp;
      doc["humidity"] = hum;
      doc["ts"] = millis();
      doc["ip"] = WiFi.localIP().toString();

      char buffer[200];
      serializeJson(doc, buffer);

      if (client.publish(MQTT_TOPIC, buffer)) {
        Serial.println("📤 " + String(buffer));
      } else {
        Serial.println("❌ Error enviando datos");
      }
      
      lastSend = millis();
    }
  }
}
