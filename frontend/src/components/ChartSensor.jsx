import { Line } from "react-chartjs-2";
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
} from "chart.js";
import { chartOptions } from "./chartConfig";

ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend
);

const ChartSensor = ({ labels, temperatureData, humidityData }) => {
  const data = {
    labels,
    datasets: [
      {
        label: "Temperatura (°C)",
        data: temperatureData,
        borderColor: "#ff6384",
        backgroundColor: "rgba(255,99,132,0.2)",
        yAxisID: 'y',
      },
      {
        label: "Humedad (%)",
        data: humidityData,
        borderColor: "#36a2eb",
        backgroundColor: "rgba(54,162,235,0.2)",
        yAxisID: 'y1',
      },
    ],
  };

  return <Line data={data} options={chartOptions} />;
};

export default ChartSensor;
