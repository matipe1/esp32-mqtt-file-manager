import express from "express";
import cors from "cors";
import dataRoutes from "./routes/dataRoutes.js";

const PORT = process.env.PORT || 3000;

const app = express();
app.use(cors());

app.use(dataRoutes);

app.listen(PORT, () => console.log(`API corriendo en http://localhost:${PORT}`));