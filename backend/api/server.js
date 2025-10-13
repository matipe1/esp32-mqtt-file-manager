import express from "express";
import fs from "fs";
import path from "path";
import {fileURLToPath} from "url";
import cors from "cors";

const PORT = process.env.PORT || 3000;

const app = express();
app.use(cors());

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const csvPath = path.join(__dirname, "../data/data.csv");

app.get("/mediciones/th", (req, res) => {
    try {
    if (!fs.existsSync(csvPath)) {
        return res.status(404).json({ error: "Archivo CSV no encontrado" });
    }
    const csv = fs.readFileSync(csvPath, "utf-8");
    if (!csv.trim()) return res.json([]);

    const rows = csv.trim().split("\n").map(line => {
        const [device, temp, hum, ts, timestamp] = line.split(",");
        return { device, temperature: +temp, humidity: +hum, ts, timestamp};
    });
    res.json(rows);
    } catch (err) {
        console.error("Error al leer el CSV:", err);
        res.status(500).json({ error: "Error al leer el archivo CSV" });
    }
});

app.listen(PORT, () => console.log(`API corriendo en http://localhost:${PORT}`));