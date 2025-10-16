#!/bin/bash

# Script para configurar mDNS/Avahi para el broker MQTT
# Ejecutar con sudo: sudo ./setup-mdns.sh

echo "Configurando mDNS para MQTT broker..."

# Verificar si Avahi está instalado
if ! command -v avahi-daemon &> /dev/null; then
    echo "Instalando Avahi (mDNS)..."
    apt-get update
    apt-get install -y avahi-daemon avahi-utils
fi

# Crear archivo de servicio mDNS para MQTT
cat > /etc/avahi/services/mqtt.service << EOF
<?xml version="1.0" standalone='no'?>
<!DOCTYPE service-group SYSTEM "avahi-service.dtd">
<service-group>
  <name replace-wildcards="yes">MQTT Broker en %h</name>
  <service>
    <type>_mqtt._tcp</type>
    <port>1883</port>
    <txt-record>description=MQTT Broker para ESP32</txt-record>
  </service>
</service-group>
EOF

# Crear alias para resolución directa
cat > /etc/avahi/services/mqtt-alias.service << EOF
<?xml version="1.0" standalone='no'?>
<!DOCTYPE service-group SYSTEM "avahi-service.dtd">
<service-group>
  <name>mqtt-broker</name>
  <service>
    <type>_mqtt._tcp</type>
    <port>1883</port>
    <host-name>mqtt-broker.local</host-name>
  </service>
</service-group>
EOF

# Reiniciar servicio Avahi
systemctl restart avahi-daemon

# Verificar configuración
echo "Verificando configuración mDNS..."
sleep 2
avahi-browse -t _mqtt._tcp

echo "✅ Configuración completada!"
echo "El broker MQTT ahora es accesible como: mqtt-broker.local"
echo "Verifica con: avahi-resolve -n mqtt-broker.local"
