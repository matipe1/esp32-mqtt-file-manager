import React from "react";
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

  const options = {
    responsive: true,
    interaction: {
      mode: 'index',
      intersect: false,
    },
    stacked: false,
    plugins: {
      legend: {
        position: 'top',
      },
      title: {
        display: true,
        text: 'Lecturas de Temperatura y Humedad',
      },
    },
    scales: {
      y: {
        type: 'linear',
        display: true,
        position: 'left',
        title: {
          display: true,
          text: 'Temperatura (°C)',
        },
      },
      y1: {
        type: 'linear',
        display: true,
        position: 'right',
        grid: {
          drawOnChartArea: false,
        },
        title: {
          display: true,
          text: 'Humedad (%)',
        },
      },
    },
  };

  return <Line data={data} options={options} />;
};

export default ChartSensor;
