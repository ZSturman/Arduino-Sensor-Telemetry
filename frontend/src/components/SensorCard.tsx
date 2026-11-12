import type { SensorCardViewModel } from "../lib/telemetry";
import { Sparkline } from "./Sparkline";

interface SensorCardProps {
  card: SensorCardViewModel;
}

function renderMetricValue(value: unknown) {
  if (value === undefined) {
    return "Waiting";
  }
  if (typeof value === "boolean") {
    return value ? "true" : "false";
  }
  return String(value);
}

export function SensorCard({ card }: SensorCardProps) {
  return (
    <article className="sensor-card">
      <header className="sensor-card__header">
        <div>
          <h3>{card.name}</h3>
          <p>{card.purpose}</p>
        </div>
        <span className="sensor-card__status">
          {String(card.status.healthy ?? "unknown")}
        </span>
      </header>

      <div className="sensor-card__metrics">
        {card.fields.map((field) => (
          <div key={field.key} className="metric-tile">
            <span className="metric-tile__label">{field.key}</span>
            <strong className="metric-tile__value">{renderMetricValue(field.value)}</strong>
            <Sparkline points={field.history} />
          </div>
        ))}
      </div>

      <p className="sensor-card__footer">{card.appearance}</p>
    </article>
  );
}

