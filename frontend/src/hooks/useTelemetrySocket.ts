import { useEffect, useRef } from "react";

import { getTelemetrySocketUrl } from "../lib/api";
import type { SocketMessage } from "../types/telemetry";

interface Options {
  onMessage: (message: SocketMessage) => void;
  onError: (error: string) => void;
}

export function useTelemetrySocket({ onMessage, onError }: Options): void {
  const onMessageRef = useRef(onMessage);
  const onErrorRef = useRef(onError);

  useEffect(() => {
    onMessageRef.current = onMessage;
    onErrorRef.current = onError;
  }, [onError, onMessage]);

  useEffect(() => {
    const socket = new WebSocket(getTelemetrySocketUrl());

    socket.onmessage = (event) => {
      try {
        onMessageRef.current(JSON.parse(event.data) as SocketMessage);
      } catch {
        onErrorRef.current("Failed to parse WebSocket payload.");
      }
    };

    socket.onerror = () => {
      onErrorRef.current("WebSocket connection error.");
    };

    return () => socket.close();
  }, []);
}
