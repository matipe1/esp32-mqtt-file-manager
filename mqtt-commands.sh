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
        mosquitto_pub -h $LOCAL_IP -t 'test' -m "Test desde $HOSTNAME - $(date '+%Y-%m-%d %H:%M:%S')"
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
