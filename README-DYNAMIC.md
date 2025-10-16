# ESP32 MQTT File Manager - Configuración Dinámica de Red

Este proyecto ahora incluye **configuración automática de red** para que el broker MQTT funcione en cualquier red local sin necesidad de configurar IPs manualmente.

## 🚀 Inicio Rápido

```bash
# Configurar e iniciar todo el sistema automáticamente
./start-system.sh

# O por pasos:
./start-system.sh setup   # Solo configurar
./start-system.sh start   # Solo iniciar servicios
```

## 📋 Características Nuevas

### ✅ Configuración Automática
- **Detección automática de IP local**
- **Configuración de Mosquitto para red LAN**
- **Múltiples opciones de código ESP32**
- **Panel web de configuración**

### 🔧 3 Versiones del Código ESP32

#### 1. **main.cpp** - Con mDNS (Recomendado para desarrollo)
```cpp
// Usa mDNS para resolver "mqtt-broker.local"
// Fallback automático a IP fija si mDNS falla
const char* mqtt_hostname = "mqtt-broker.local";
```

#### 2. **main_dynamic.cpp** - Detección Automática de Red
```cpp
// Escanea automáticamente la red buscando brokers MQTT
// Funciona en cualquier red sin configuración
String detectarBrokerMQTT();
```

#### 3. **main_wifimanager.cpp** - Configuración Web Completa
```cpp
// Portal web para configurar WiFi y MQTT
// Interfaz web en: http://esp32-mqtt.local
// Configuración persistente en memoria
```

## 🌐 Arquitectura del Sistema

```
Internet/Router
     │
┌────┴─────┐
│   Red    │ 192.168.100.x
│   Local  │
└────┬─────┘
     │
├─ 💻 PC (Backend + Mosquitto)
│    ├─ Mosquitto Broker: :1883
│    ├─ Node.js Backend: :3000  
│    └─ React Frontend: :5173
│
└─ 📡 ESP32 (Sensor)
     ├─ WiFi Auto-config
     ├─ MQTT Auto-discovery
     └─ Web Panel: http://esp32-mqtt.local
```

## ⚙️ Configuración por Tipo de Red

### 🏠 Red Doméstica (192.168.1.x)
```bash
# El sistema detecta automáticamente
# No requiere configuración manual
./start-system.sh
```

### 🏢 Red Corporativa/Universidad
```bash
# 1. Ejecutar auto-configuración
./start-system.sh setup

# 2. Usar main_wifimanager.cpp en ESP32
# 3. Conectar a "ESP32-MQTT-Config" para configurar WiFi
```

### 🔧 Configuración Manual
```bash
# 1. Editar .env manualmente
echo "BROKER_URL=mqtt://192.168.X.X:1883" > backend/.env

# 2. Usar main.cpp y cambiar:
const char* mqtt_server = "192.168.X.X";
```

## 📱 Uso del Sistema

### 1. Configurar Sistema
```bash
./start-system.sh
```

### 2. Programar ESP32
- **Opción A**: Usar `main_wifimanager.cpp` (más fácil)
- **Opción B**: Usar `main_dynamic.cpp` (automático)  
- **Opción C**: Usar `main.cpp` (con mDNS)

### 3. Monitorear Datos
```bash
# Ver mensajes MQTT en tiempo real
./mqtt-commands.sh listen

# Ver estado del sistema
./mqtt-commands.sh status

# Enviar mensaje de prueba
./mqtt-commands.sh test
```

### 4. Acceder a Interfaces Web
- **Backend API**: http://TU_IP:3000
- **Frontend**: http://TU_IP:5173  
- **ESP32 Config**: http://esp32-mqtt.local

## 🛠️ Comandos Útiles

```bash
# Sistema
./start-system.sh full           # Configurar e iniciar todo
./mqtt-commands.sh listen        # Escuchar mensajes MQTT
./mqtt-commands.sh status        # Ver estado del sistema

# Desarrollo
cd backend && npm run dev        # Iniciar backend + MQTT
cd frontend && npm run dev       # Iniciar frontend React

# Mosquitto
sudo systemctl status mosquitto # Estado del broker
mosquitto_sub -h localhost -t '#' # Escuchar todos los topics
```

## 🔍 Solución de Problemas

### ESP32 no conecta al MQTT
```bash
# 1. Verificar que Mosquitto está corriendo
./mqtt-commands.sh status

# 2. Verificar configuración de red
ip route get 8.8.8.8 | grep -oP 'src \K\S+'

# 3. Usar main_wifimanager.cpp para diagnóstico web
```

### Cambio de Red WiFi
```bash
# 1. El sistema detecta automáticamente la nueva IP
./backend/auto-config.sh

# 2. Para ESP32 con WiFiManager:
# - Resetear configuración WiFi en el código
# - O conectar a "ESP32-MQTT-Config" para reconfigurar
```

### Firewall/Puerto Bloqueado
```bash
# Verificar puerto 1883
sudo netstat -tlnp | grep :1883

# Abrir puerto si es necesario
sudo ufw allow 1883
```

## 📊 Monitoreo y Logs

```bash
# Logs del sistema
sudo journalctl -u mosquitto -f   # Logs de Mosquitto
./mqtt-commands.sh listen         # Mensajes MQTT en vivo

# Estructura de datos
{
  "device": "esp32-01",
  "temperature": 25.6,
  "humidity": 60.2,  
  "ts": 12345678,
  "ip": "192.168.100.X"    // IP del ESP32
}
```

## 🚀 Próximos Pasos

1. **Ejecutar**: `./start-system.sh`
2. **Programar ESP32** con el código que prefieras
3. **Verificar datos**: `./mqtt-commands.sh listen`
4. **Acceder a frontend**: http://TU_IP:5173

¡El sistema ahora es completamente dinámico y funciona en cualquier red! 🎉
