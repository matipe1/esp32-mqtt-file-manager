import mqtt from "mqtt";
import fs from "fs";
import path from "path";
import {fileURLToPath} from "url";
import dotenv from "dotenv";

// Cargar variables de entorno
dotenv.config();

const brokerUrl = process.env.BROKER_URL || "mqtt://localhost:1883";
const client = mqtt.connect(brokerUrl);

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const csvPath = path.join(__dirname, "../data/data.csv");

client.on("connect", () => {
  console.log("Conectado al broker MQTT");
  client.subscribe("th/mediciones");
});

client.on("message", (topic, message) => {
    try {
        console.log(`📩 Mensaje recibido en tópico: ${topic}`);
        const data = JSON.parse(message.toString());
        // Agregar timestamp legible -> a chequear
        const timestamp = new Date().toISOString();
        const line = `${data.device},${data.temperature},${data.humidity},${data.ts},${timestamp}\n`;
        fs.appendFileSync(csvPath, line);
        console.log(`Datos guardados: ${line.trim()}`);
    } catch (err) {
        console.error("Error procesando mensaje MQTT:", err.message);
    }
});
