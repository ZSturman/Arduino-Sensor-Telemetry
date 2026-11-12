from __future__ import annotations

from contextlib import asynccontextmanager

from fastapi import FastAPI, HTTPException, WebSocket, WebSocketDisconnect
from fastapi.middleware.cors import CORSMiddleware

from app.core.settings import get_settings
from app.models.connection import ConnectRequest
from app.services.runtime import TelemetryRuntime


def create_app(runtime: TelemetryRuntime | None = None) -> FastAPI:
    settings = get_settings()
    runtime = runtime or TelemetryRuntime(settings)

    @asynccontextmanager
    async def lifespan(app: FastAPI):
        app.state.runtime = runtime
        await runtime.startup()
        try:
            yield
        finally:
            await runtime.shutdown()

    app = FastAPI(
        title="Arduino Mega Telemetry Backend",
        version="0.1.0",
        lifespan=lifespan,
    )
    app.add_middleware(
        CORSMiddleware,
        allow_origins=["*"],
        allow_credentials=True,
        allow_methods=["*"],
        allow_headers=["*"],
    )

    @app.get("/api/health")
    async def health() -> dict:
        return runtime.health_payload()

    @app.get("/api/transports")
    async def transports() -> list[dict]:
        return [item.model_dump(mode="json") for item in await runtime.list_transports()]

    @app.get("/api/transports/{transport_id}/endpoints")
    async def transport_endpoints(transport_id: str) -> list[dict]:
        return [item.model_dump(mode="json") for item in await runtime.list_endpoints(transport_id)]

    @app.get("/api/connection")
    async def connection() -> dict:
        return runtime.state.model_dump(mode="json")

    @app.post("/api/connection/connect")
    async def connect(request: ConnectRequest) -> dict:
        try:
            state = await runtime.connect(request)
        except ValueError as exc:
            raise HTTPException(status_code=400, detail=str(exc)) from exc
        return state.model_dump(mode="json")

    @app.post("/api/connection/disconnect")
    async def disconnect() -> dict:
        state = await runtime.disconnect()
        return state.model_dump(mode="json")

    @app.get("/api/catalog/sensors")
    async def catalog() -> dict:
        return runtime.catalog.payload

    @app.get("/api/telemetry/recent")
    async def recent() -> dict:
        return runtime.store.snapshot().model_dump(mode="json")

    @app.websocket("/ws/telemetry")
    async def telemetry_socket(websocket: WebSocket) -> None:
        await runtime.websocket_hub.connect(websocket)
        await websocket.send_json(
            {"type": "connection_state", "payload": runtime.state.model_dump(mode="json")}
        )
        await websocket.send_json(
            {"type": "telemetry_snapshot", "payload": runtime.store.snapshot().model_dump(mode="json")}
        )
        try:
            while True:
                await websocket.receive_text()
        except WebSocketDisconnect:
            runtime.websocket_hub.disconnect(websocket)

    return app


app = create_app()
