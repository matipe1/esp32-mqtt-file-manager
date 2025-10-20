import { useState, useEffect } from 'react';
import { formatTimestamp } from '../utils/formatData';

export default function useSensorData() {
  const [labels, setLabels] = useState([]);
  const [temperatureData, setTemperatureData] = useState([]);
  const [humidityData, setHumidityData] = useState([]);

  useEffect(() => {
    const fetchData = async () => {
      try {
        const response = await fetch('http://localhost:3000/mediciones/th');
        const data = await response.json();

        if (Array.isArray(data)) {
          setLabels(data.map(item => formatTimestamp(item.timestamp)));
          setTemperatureData(data.map(item => item.temperature));
          setHumidityData(data.map(item => item.humidity));
        } else {
          console.error("Formato de respuesta inesperado:", data);
        }
      } catch (error) {
        console.error("Error al obtener mediciones:", error);
      }
    };

    fetchData(); // llamada inicial

    const interval = setInterval(fetchData, 5000); // refresco cada 5s
    return () => clearInterval(interval); // limpiar interval al desmontar
  }, []); // array vacío, se ejecuta solo al montar

  return { labels, temperatureData, humidityData };
}