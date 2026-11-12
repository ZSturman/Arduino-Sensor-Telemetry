from fastapi.testclient import TestClient

from app.main import create_app


def test_health_and_catalog_routes() -> None:
    app = create_app()
    client = TestClient(app)

    health = client.get("/api/health")
    assert health.status_code == 200
    assert health.json()["status"] == "ok"

    catalog = client.get("/api/catalog/sensors")
    assert catalog.status_code == 200
    assert len(catalog.json()["sensors"]) >= 10


def test_connection_to_simulator() -> None:
    app = create_app()
    client = TestClient(app)

    response = client.post(
        "/api/connection/connect",
        json={
            "transport": "simulator",
            "endpoint": "simulator://default",
            "baud_rate": 115200,
            "auto_reconnect": False,
            "logging_mode": "off",
            "simulator_profile": "default",
        },
    )
    assert response.status_code == 200
    assert response.json()["connection_state"] == "connected"
