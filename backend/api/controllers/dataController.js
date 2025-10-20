import { readCSV } from "../services/csvService.js";

export function getMedicionesTH(req, res) {
  try {
    const rows = readCSV();
    res.json(rows);
  } catch (err) {
    console.error("Error al leer el CSV:", err);
    res.status(500).json({ error: "Error al leer el archivo CSV" });
  }
}
