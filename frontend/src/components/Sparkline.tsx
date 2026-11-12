import type { HistoryPoint } from "../types/telemetry";

interface SparklineProps {
  points: HistoryPoint[];
}

export function Sparkline({ points }: SparklineProps) {
  if (points.length < 2) {
    return <div className="sparkline sparkline--empty">Waiting for history</div>;
  }

  const width = 240;
  const height = 64;
  const values = points.map((point) => point.value);
  const min = Math.min(...values);
  const max = Math.max(...values);
  const range = Math.max(max - min, 1);

  const path = points
    .map((point, index) => {
      const x = (index / (points.length - 1)) * width;
      const y = height - ((point.value - min) / range) * height;
      return `${index === 0 ? "M" : "L"}${x.toFixed(1)},${y.toFixed(1)}`;
    })
    .join(" ");

  return (
    <svg className="sparkline" viewBox={`0 0 ${width} ${height}`} role="img" aria-label="Metric history">
      <path d={path} fill="none" stroke="currentColor" strokeWidth="3" strokeLinecap="round" />
    </svg>
  );
}

