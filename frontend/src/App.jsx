import ChartSensor from './components/ChartSensor';
import useSensorData from './hooks/useSensorData';

function App() {
  const { labels, temperatureData, humidityData } = useSensorData();

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

export default App;
