import express from "express";
import { getMedicionesTH } from "../controllers/dataController.js";

const router = express.Router();

router.get("/mediciones/th", getMedicionesTH);

export default router;
