#!/bin/bash

# Script principal para configurar y ejecutar el sistema MQTT completo
# Uso: ./start-system.sh [opcion]
# Opciones: 
#   setup    - Solo configurar (primera vez)
#   start    - Solo iniciar servicios
#   full     - Configurar e iniciar (por defecto)

set -e

OPTION=${1:-full}
PROJECT_DIR="/home/estudiante/TA/esp32-mqtt-file-manager"

echo "🚀 Iniciando sistema ESP32-MQTT-File-Manager"
echo "Opción: $OPTION"

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Función para logging
log() {
    echo -e "${BLUE}[$(date +'%H:%M:%S')]${NC} $1"
}

success() {
    echo -e "${GREEN}✅ $1${NC}"
}

warning() {
    echo -e "${YELLOW}⚠️ $1${NC}"
}

error() {
    echo -e "${RED}❌ $1${NC}"
    exit 1
}

# Verificar si Mosquitto está instalado
check_mosquitto() {
    if ! command -v mosquitto &> /dev/null; then
        warning "Mosquitto no está instalado"
        read -p "¿Instalar Mosquitto? (y/n): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            log "Instalando Mosquitto..."
            sudo apt update
            sudo apt install -y mosquitto mosquitto-clients
            success "Mosquitto instalado"
        else
            error "Mosquitto es necesario para el sistema"
        fi
    else
        success "Mosquitto ya está instalado"
    fi
}

# Configurar Mosquitto para aceptar conexiones de red
configure_mosquitto() {
    log "Configurando Mosquitto..."
    
    # Backup de configuración existente
    if [ -f /etc/mosquitto/mosquitto.conf ]; then
        sudo cp /etc/mosquitto/mosquitto.conf /etc/mosquitto/mosquitto.conf.backup
    fi
    
    # Crear configuración que permite conexiones de red
    sudo tee /etc/mosquitto/conf.d/local.conf > /dev/null << EOF
# Configuración para ESP32-MQTT-File-Manager
listener 1883 0.0.0.0
allow_anonymous true
log_dest stdout
log_type all
connection_messages true
EOF
    
    success "Mosquitto configurado para red local"
}

# Iniciar servicios
start_services() {
    log "Iniciando servicios..."
    
    # Reiniciar Mosquitto con nueva configuración
    sudo systemctl restart mosquitto
    sudo systemctl enable mosquitto
    
    # Verificar que Mosquitto está corriendo
    if systemctl is-active --quiet mosquitto; then
        success "Mosquitto ejecutándose"
        local_ip=$(ip route get 8.8.8.8 | grep -oP 'src \K\S+')
        log "Broker MQTT disponible en: $local_ip:1883"
    else
        error "Error iniciando Mosquitto"
    fi
    
    # Ir al directorio del backend
    cd "$PROJECT_DIR/backend"
    
    # Instalar dependencias si no existen
    if [ ! -d "node_modules" ]; then
        log "Instalando dependencias de Node.js..."
        npm install
    fi
    
    # Crear .env si no existe
    if [ ! -f ".env" ]; then
        ./auto-config.sh
    fi
    
    success "Backend configurado"
}

# Función para mostrar información del sistema
show_info() {
    echo
    echo "📋 === INFORMACIÓN DEL SISTEMA ==="
    
    local_ip=$(ip route get 8.8.8.8 | grep -oP 'src \K\S+')
    echo "🌐 IP Local: $local_ip"
    echo "📡 Broker MQTT: $local_ip:1883"
    echo "🖥️ Panel Web: http://$local_ip:3000"
    
    echo
    echo "📱 === CONFIGURACIÓN ESP32 ==="
    echo "1. Opción A - Código automático:"
    echo "   - Usar: src/main_wifimanager.cpp"
    echo "   - Conectar a WiFi 'ESP32-MQTT-Config' para configurar"
    echo "   - Panel web automático en: http://esp32-mqtt.local"
    
    echo
    echo "2. Opción B - IP fija:"
    echo "   - Usar: src/main.cpp (ya configurado con mDNS)"
    echo "   - Cambiar mqtt_server a: \"$local_ip\""
    
    echo
    echo "3. Opción C - Detección automática:"
    echo "   - Usar: src/main_dynamic.cpp"
    echo "   - Busca automáticamente el broker en la red"
    
    echo
    echo "🚀 === COMANDOS ÚTILES ==="
    echo "• Iniciar backend: cd backend && npm run dev"
    echo "• Iniciar frontend: cd frontend && npm run dev"
    echo "• Ver logs MQTT: mosquitto_sub -h $local_ip -t 'th/mediciones'"
    echo "• Test MQTT: mosquitto_pub -h $local_ip -t 'test' -m 'Hello MQTT'"
    echo
}

# Crear archivo de comandos rápidos
create_shortcuts() {
    cat > "$PROJECT_DIR/mqtt-commands.sh" << 'EOF'
#!/bin/bash
# Comandos útiles para el sistema MQTT

LOCAL_IP=$(ip route get 8.8.8.8 | grep -oP 'src \K\S+')

case $1 in
    "logs")
        echo "📄 Logs de Mosquitto:"
        sudo journalctl -u mosquitto -f
        ;;
    "listen")
        echo "👂 Escuchando mensajes MQTT:"
        mosquitto_sub -h $LOCAL_IP -t 'th/mediciones'
        ;;
    "test")
        echo "🧪 Enviando mensaje de prueba:"
        mosquitto_pub -h $LOCAL_IP -t 'test' -m "Test desde $HOSTNAME - $(date)"
        ;;
    "status")
        echo "📊 Estado del sistema:"
        echo "Mosquitto: $(systemctl is-active mosquitto)"
        echo "IP: $LOCAL_IP"
        netstat -tlnp | grep :1883 || echo "Puerto 1883 no está escuchando"
        ;;
    *)
        echo "Uso: $0 [logs|listen|test|status]"
        ;;
esac
EOF
    chmod +x "$PROJECT_DIR/mqtt-commands.sh"
    success "Script de comandos creado: mqtt-commands.sh"
}

# Función principal
main() {
    case $OPTION in
        "setup")
            check_mosquitto
            configure_mosquitto
            start_services
            create_shortcuts
            ;;
        "start")
            start_services
            ;;
        "full"|*)
            check_mosquitto
            configure_mosquitto
            start_services
            create_shortcuts
            show_info
            ;;
    esac
    
    echo
    success "¡Sistema listo!"
    echo "💡 Tip: Usa './mqtt-commands.sh listen' para ver los mensajes en tiempo real"
}

# Ejecutar función principal
main
