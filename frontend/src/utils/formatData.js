// Funciones auxiliares para formatear datos

export function formatTimestamp(ts) {
  // Ejemplo: convertir '2025-10-20T10:00:01Z' a '10:00:01'
  const date = new Date(ts);
  return date.toLocaleTimeString();
}
