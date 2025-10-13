import { useEffect, useState } from 'react';
import ChartSensor from './components/ChartSensor';

function App() {
  // Datos mock para pruebas
  const [labels, setLabels] = useState([]);
  const [temperatureData, setTemperatureData] = useState([]);
  const [humidityData, setHumidityData] = useState([]);

  // Usar axios
  // Plantear un servicio pequeño que me permita consumir la api

  useEffect(() => {
    let intervalId;

    const fetchData = async () => {
      try {
        // Endpoint real del backend
        const response = await fetch('http://localhost:3000/mediciones/th');
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        const data = await response.json();

        // Verificamos si el backend envía un array o un objeto con campo 'data'
        const mediciones = Array.isArray(data) ? data : data.data || [];

        setLabels(
          mediciones.map(d => {
            const fecha = new Date(Number(d.ts));
            return fecha.toLocaleTimeString(); // Muestra 12:34:56
          })
        );

        // Si el backend devuelve [{ device, temperature, humidity, ts }]
        setTemperatureData(mediciones.map(d => d.temperature));
        setHumidityData(mediciones.map(d => d.humidity));
      } catch (error) {
        console.error('Error al obtener datos del backend:', error);
      }
    };

    fetchData();
    intervalId = setInterval(fetchData, 5000); // Actualiza cada 5 segundos

    return () => clearInterval(intervalId);
  }, []);

  return (
    <div style={{ maxWidth: 800, margin: '0 auto', padding: 24 }}>
      <h1>ESP32 Sensor Dashboard</h1>
      <ChartSensor
        labels={labels}
        temperatureData={temperatureData}
        humidityData={humidityData}
      />
    </div>
  );
}

export default App
