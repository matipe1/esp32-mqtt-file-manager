// Configuración y opciones para Chart.js

export const chartOptions = {
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
      ticks: {
        padding: 5,
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
      ticks: {
        padding: 5,
      },
    },
    x: {
      ticks: {
        autoSkip: true,        // omite algunas etiquetas para no saturar
        maxTicksLimit: 10,     // máximo de etiquetas visibles
        maxRotation: 45,       // rotación máxima de la etiqueta
        minRotation: 30,       // rotación mínima
      },
    },
  },
};
