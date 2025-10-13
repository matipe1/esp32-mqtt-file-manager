
import React, { useEffect, useState } from 'react';
import ChartSensor from './components/ChartSensor';

function App() {
  // Datos mock para pruebas
  const [labels, setLabels] = useState([
    '10:00', '10:05', '10:10', '10:15', '10:20', '10:25', '10:30'
  ]);
  const [temperatureData, setTemperatureData] = useState([23, 24, 24.5, 25, 24.8, 25.2, 25]);
  const [humidityData, setHumidityData] = useState([55, 56, 57, 58, 57.5, 56.8, 56]);

  // Cuando el backend esté listo, descomentar y ajustar la URL:
  /*
  useEffect(() => {
    const fetchData = async () => {
      try {
        const response = await fetch('http://localhost:3000/api/datos'); // Ajusta la URL según tu backend
        const data = await response.json();
        // Suponiendo que el backend devuelve un array de objetos { timestamp, temperatura, humedad }
        setLabels(data.map(d => d.timestamp));
        setTemperatureData(data.map(d => d.temperatura));
        setHumidityData(data.map(d => d.humedad));
      } catch (error) {
        console.error('Error al obtener datos del backend:', error);
      }
    };
    fetchData();
    // Si quieres actualizar en tiempo real, puedes usar setInterval aquí
  }, []);
  */

  return (
    <div style={{ maxWidth: 800, margin: '0 auto', padding: 24 }}>
      <h1>ESP32 Sensor Dashboard</h1>
      <ChartSensor labels={labels} temperatureData={temperatureData} humidityData={humidityData} />
    </div>
  );
}

export default App
