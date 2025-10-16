#!/bin/bash

# Script para detectar automáticamente la IP de la interfaz de red
# y configurar el broker MQTT

echo "🔍 Detectando configuración de red..."

# Detectar IP de la interfaz principal (excluyendo loopback)
LOCAL_IP=$(ip route get 8.8.8.8 | grep -oP 'src \K\S+')

if [ -z "$LOCAL_IP" ]; then
    echo "❌ No se pudo detectar la IP local"
    exit 1
fi

echo "📍 IP detectada: $LOCAL_IP"

# Crear archivo de configuración
cat > .env << EOF
# Configuración automática generada
BROKER_URL=mqtt://$LOCAL_IP:1883
LOCAL_IP=$LOCAL_IP
MQTT_PORT=1883
EOF

echo "✅ Archivo .env creado con:"
cat .env

echo ""
echo "📋 Pasos siguientes:"
echo "1. Instalar Mosquitto: sudo apt install mosquitto mosquitto-clients"
echo "2. Configurar Mosquitto para escuchar en todas las interfaces"
echo "3. Actualizar el código ESP32 con la IP: $LOCAL_IP"
echo "4. Ejecutar: npm run dev"
