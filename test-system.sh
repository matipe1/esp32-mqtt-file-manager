#!/bin/bash

# Script de prueba completa del sistema ESP32-MQTT
# Simula datos del ESP32 y verifica el flujo completo

LOCAL_IP=$(ip route get 8.8.8.8 | grep -oP 'src \K\S+')
TOPIC="th/mediciones"

echo "🧪 PRUEBA COMPLETA DEL SISTEMA ESP32-MQTT"
echo "=========================================="
echo "IP del broker: $LOCAL_IP"
echo "Tópico: $TOPIC"
echo

# Función para simular datos del ESP32
simular_esp32() {
    local device_id="esp32-test-$(date +%s)"
    local count=1
    
    echo "📡 Simulando ESP32: $device_id"
    echo "Enviando 5 mensajes de prueba..."
    
    while [ $count -le 5 ]; do
        # Generar datos simulados
        local temp=$(echo "scale=1; 20 + ($count * 2.5)" | bc)
        local hum=$(echo "scale=1; 50 + ($count * 3)" | bc)
        local timestamp=$(date +%s)
        
        # Crear JSON
        local json="{\"device\":\"$device_id\",\"temperature\":$temp,\"humidity\":$hum,\"ts\":$timestamp,\"ip\":\"192.168.100.200\"}"
        
        echo "📤 [$count/5] $json"
        
        # Publicar mensaje
        mosquitto_pub -h $LOCAL_IP -t $TOPIC -m "$json"
        
        count=$((count + 1))
        sleep 2
    done
    
    echo "✅ Simulación completada"
}

# Función para escuchar mensajes
escuchar_mensajes() {
    echo "👂 Escuchando mensajes MQTT por 15 segundos..."
    timeout 15s mosquitto_sub -h $LOCAL_IP -t $TOPIC -v
}

# Función para verificar archivo CSV
verificar_csv() {
    local csv_file="/home/estudiante/TA/esp32-mqtt-file-manager/backend/data/data.csv"
    
    echo "📄 Verificando archivo CSV: $csv_file"
    
    if [ -f "$csv_file" ]; then
        echo "✅ Archivo CSV existe"
        echo "📊 Últimas 5 líneas:"
        tail -5 "$csv_file"
        echo
        echo "📈 Total de registros: $(wc -l < "$csv_file")"
    else
        echo "❌ Archivo CSV no encontrado"
    fi
}

# Función para probar la API
probar_api() {
    echo "🌐 Probando API del backend..."
    
    # Verificar si la API está corriendo
    if curl -s "http://$LOCAL_IP:3000/health" > /dev/null 2>&1; then
        echo "✅ API respondiendo en http://$LOCAL_IP:3000"
        
        # Probar endpoint de datos
        echo "📊 Datos desde API:"
        curl -s "http://$LOCAL_IP:3000/api/data" | head -c 200
        echo "..."
    else
        echo "⚠️ API no está corriendo en puerto 3000"
        echo "💡 Inicia con: cd backend && npm run dev"
    fi
}

# Función principal
main() {
    case ${1:-full} in
        "mqtt")
            simular_esp32
            ;;
        "listen")
            escuchar_mensajes
            ;;
        "csv")
            verificar_csv
            ;;
        "api")
            probar_api
            ;;
        "full"|*)
            echo "1. Probando conexión MQTT..."
            simular_esp32 &
            
            echo
            echo "2. Escuchando respuestas..."
            sleep 2
            escuchar_mensajes
            
            echo
            echo "3. Verificando almacenamiento..."
            verificar_csv
            
            echo
            echo "4. Probando API..."
            probar_api
            
            echo
            echo "🎉 Prueba completa terminada!"
            ;;
    esac
}

# Mostrar ayuda si se pide
if [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    echo "Uso: $0 [opción]"
    echo "Opciones:"
    echo "  full    - Prueba completa (por defecto)"
    echo "  mqtt    - Solo simular ESP32"
    echo "  listen  - Solo escuchar mensajes"
    echo "  csv     - Solo verificar CSV"
    echo "  api     - Solo probar API"
    exit 0
fi

# Verificar dependencias
if ! command -v mosquitto_pub &> /dev/null; then
    echo "❌ mosquitto_pub no encontrado. Instala con: sudo apt install mosquitto-clients"
    exit 1
fi

if ! command -v bc &> /dev/null; then
    echo "❌ bc no encontrado. Instala con: sudo apt install bc"
    exit 1
fi

# Ejecutar función principal
main "$1"
