import type {
  HistoryPoint,
  RecentTelemetryResponse,
  SensorCatalogEntry,
  SensorCatalogPayload,
  SocketMessage,
  TelemetryEnvelope
} from "../types/telemetry";

export interface SensorCardViewModel {
  id: string;
  name: string;
  purpose: string;
  fields: Array<{
    key: string;
    value: unknown;
    history: HistoryPoint[];
  }>;
  status: Record<string, unknown>;
  appearance: string;
}

export function emptyRecentTelemetry(): RecentTelemetryResponse {
  return {
    latest_metrics: {},
    sensor_status: {},
    numeric_history: {},
    recent_events: [],
    raw_console: [],
    latest_message: null
  };
}

export function mergeSocketMessage(
  current: RecentTelemetryResponse,
  message: SocketMessage
): RecentTelemetryResponse {
  if (message.type === "telemetry_snapshot") {
    return message.payload;
  }

  if (message.type === "parse_error") {
    return {
      ...current,
      raw_console: [...current.raw_console.slice(-99), `${message.payload.raw_line}    // parse_error: ${message.payload.error}`]
    };
  }

  if (message.type !== "envelope") {
    return current;
  }

  return mergeEnvelope(current, message.payload);
}

export function mergeEnvelope(
  current: RecentTelemetryResponse,
  envelope: TelemetryEnvelope
): RecentTelemetryResponse {
  const rawConsole = envelope.raw_line
    ? [...current.raw_console.slice(-99), envelope.raw_line]
    : current.raw_console;

  if (envelope.message_type === "event") {
    return {
      ...current,
      recent_events: [...current.recent_events.slice(-49), envelope],
      raw_console: rawConsole,
      latest_message: envelope
    };
  }

  const nextHistory = { ...current.numeric_history };
  Object.entries(envelope.metrics ?? {}).forEach(([key, value]) => {
    if (typeof value === "number") {
      nextHistory[key] = [...(nextHistory[key] ?? []).slice(-119), { timestamp_ms: envelope.timestamp_ms, value }];
    }
    if (typeof value === "boolean") {
      nextHistory[key] = [...(nextHistory[key] ?? []).slice(-119), { timestamp_ms: envelope.timestamp_ms, value: value ? 1 : 0 }];
    }
  });

  return {
    latest_metrics: {
      ...current.latest_metrics,
      ...(envelope.metrics ?? {})
    },
    sensor_status: envelope.sensor_status ?? current.sensor_status,
    numeric_history: nextHistory,
    recent_events: current.recent_events,
    raw_console: rawConsole,
    latest_message: envelope
  };
}

export function buildSensorCards(
  catalog: SensorCatalogPayload | null,
  recent: RecentTelemetryResponse
): SensorCardViewModel[] {
  if (!catalog) {
    return [];
  }

  return catalog.sensors.map((sensor) => ({
    id: sensor.id,
    name: sensor.name,
    purpose: sensor.purpose,
    fields: sensor.expected_fields.map((fieldKey) => ({
      key: fieldKey,
      value: recent.latest_metrics[fieldKey],
      history: recent.numeric_history[fieldKey] ?? []
    })),
    status: recent.sensor_status[sensor.id] ?? {},
    appearance: sensor.frontend.appearance
  }));
}

export function getNumericMetricKeys(recent: RecentTelemetryResponse): string[] {
  return Object.keys(recent.numeric_history).sort();
}

export function getUnmappedMetrics(
  catalog: SensorCatalogPayload | null,
  recent: RecentTelemetryResponse
): Array<[string, unknown]> {
  const mapped = new Set<string>();
  catalog?.sensors.forEach((sensor) => {
    sensor.expected_fields.forEach((field) => mapped.add(field));
  });

  return Object.entries(recent.latest_metrics).filter(([key]) => !mapped.has(key));
}

export function findSensorById(
  catalog: SensorCatalogPayload | null,
  sensorId: string
): SensorCatalogEntry | undefined {
  return catalog?.sensors.find((sensor) => sensor.id === sensorId);
}

