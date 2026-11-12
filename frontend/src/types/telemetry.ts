export type ScalarValue = string | number | boolean | null;

export interface TransportDescriptor {
  id: string;
  label: string;
  description: string;
  supported: boolean;
  hardware_backed: boolean;
  next_phase?: boolean;
}

export interface TransportEndpoint {
  id: string;
  label: string;
  description: string;
  available: boolean;
  metadata: Record<string, unknown>;
}

export interface ConnectionState {
  backend_connected: boolean;
  connection_state: "disconnected" | "connecting" | "connected" | "reconnecting" | "error";
  transport: string | null;
  endpoint: string | null;
  baud_rate: number | null;
  auto_reconnect: boolean;
  malformed_messages: number;
  last_error: string | null;
  device_id: string | null;
  last_sequence: number | null;
  last_message_at: string | null;
}

export interface EventDetails {
  source: string;
  name: string;
  fields: Record<string, ScalarValue>;
}

export interface TelemetryEnvelope {
  schema_version: string;
  message_type: "telemetry" | "event";
  timestamp_ms: number;
  sequence: number;
  device_id: string;
  transport: string;
  metrics?: Record<string, ScalarValue>;
  sensor_status?: Record<string, Record<string, ScalarValue>>;
  event?: EventDetails;
  received_at?: string;
  raw_line?: string;
}

export interface HistoryPoint {
  timestamp_ms: number;
  value: number;
}

export interface RecentTelemetryResponse {
  latest_metrics: Record<string, ScalarValue>;
  sensor_status: Record<string, Record<string, ScalarValue>>;
  numeric_history: Record<string, HistoryPoint[]>;
  recent_events: TelemetryEnvelope[];
  raw_console: string[];
  latest_message: TelemetryEnvelope | null;
}

export interface SensorCatalogEntry {
  id: string;
  name: string;
  category: string;
  purpose: string;
  required_parts: string[];
  pins: Array<{
    mega_pin: string;
    module_pin: string;
    notes: string;
  }>;
  wiring_summary: string;
  breadboard_description: string;
  ascii_diagram: string;
  firmware: {
    file: string;
    module: string;
    function: string;
  };
  libraries: string[];
  expected_fields: string[];
  frontend: {
    card_title: string;
    appearance: string;
  };
  constraints: string[];
}

export interface SensorCatalogPayload {
  schema_version: string;
  board: string;
  firmware_sketch: string;
  transport_note: string;
  sensors: SensorCatalogEntry[];
}

export interface HealthPayload {
  status: string;
  backend_started_at: string;
  connection: ConnectionState;
}

export type SocketMessage =
  | { type: "connection_state"; payload: ConnectionState }
  | { type: "telemetry_snapshot"; payload: RecentTelemetryResponse }
  | { type: "envelope"; payload: TelemetryEnvelope }
  | {
      type: "parse_error";
      payload: {
        raw_line: string;
        error: string;
        malformed_messages: number;
      };
    };

