import type {
  ConnectionState,
  HealthPayload,
  RecentTelemetryResponse,
  SensorCatalogPayload,
  TransportDescriptor,
  TransportEndpoint
} from "../types/telemetry";

const BACKEND_BASE_URL = import.meta.env.VITE_BACKEND_BASE_URL ?? "http://127.0.0.1:8000";

async function fetchJson<T>(path: string, init?: RequestInit): Promise<T> {
  const response = await fetch(`${BACKEND_BASE_URL}${path}`, {
    headers: {
      "Content-Type": "application/json"
    },
    ...init
  });
  if (!response.ok) {
    throw new Error(`${response.status} ${response.statusText}`);
  }
  return (await response.json()) as T;
}

export interface ConnectRequest {
  transport: string;
  endpoint: string | null;
  baud_rate: number;
  auto_reconnect: boolean;
  logging_mode: "off" | "jsonl" | "jsonl_csv";
  simulator_profile: string;
}

export function getHealth(): Promise<HealthPayload> {
  return fetchJson<HealthPayload>("/api/health");
}

export function getTransports(): Promise<TransportDescriptor[]> {
  return fetchJson<TransportDescriptor[]>("/api/transports");
}

export function getEndpoints(transportId: string): Promise<TransportEndpoint[]> {
  return fetchJson<TransportEndpoint[]>(`/api/transports/${transportId}/endpoints`);
}

export function getConnection(): Promise<ConnectionState> {
  return fetchJson<ConnectionState>("/api/connection");
}

export function connectTelemetry(payload: ConnectRequest): Promise<ConnectionState> {
  return fetchJson<ConnectionState>("/api/connection/connect", {
    method: "POST",
    body: JSON.stringify(payload)
  });
}

export function disconnectTelemetry(): Promise<ConnectionState> {
  return fetchJson<ConnectionState>("/api/connection/disconnect", {
    method: "POST"
  });
}

export function getCatalog(): Promise<SensorCatalogPayload> {
  return fetchJson<SensorCatalogPayload>("/api/catalog/sensors");
}

export function getRecentTelemetry(): Promise<RecentTelemetryResponse> {
  return fetchJson<RecentTelemetryResponse>("/api/telemetry/recent");
}

export function getTelemetrySocketUrl(): string {
  const url = new URL(BACKEND_BASE_URL);
  url.protocol = url.protocol === "https:" ? "wss:" : "ws:";
  url.pathname = "/ws/telemetry";
  return url.toString();
}
