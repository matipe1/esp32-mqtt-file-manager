import fs from "fs";
import path from "path";
import { fileURLToPath } from "url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const csvPath = path.join(__dirname, "../../data/data.csv");

export function readCSV() {
  if (!fs.existsSync(csvPath)) {
    throw new Error("Archivo CSV no encontrado");
  }
  const csv = fs.readFileSync(csvPath, "utf-8");
  if (!csv.trim()) return [];

  return csv.trim().split("\n").map(line => {
    const [device, temp, hum, ts, timestamp] = line.split(",");
    return { device, temperature: +temp, humidity: +hum, ts, timestamp };
  });
}
