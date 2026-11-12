from app.services.ndjson import MalformedEnvelopeError, NDJSONEnvelopeParser


def test_parser_accepts_valid_telemetry() -> None:
    parser = NDJSONEnvelopeParser()
    envelope = parser.parse_line(
        '{"schema_version":"1.0.0","message_type":"telemetry","timestamp_ms":10,"sequence":1,"device_id":"mega","transport":"usb_serial","metrics":{"temperature_c":23.5},"sensor_status":{"dht11":{"connected":true,"healthy":true,"last_update_ms":10}}}'
    )
    assert envelope.message_type == "telemetry"
    assert envelope.metrics == {"temperature_c": 23.5}


def test_parser_rejects_invalid_json() -> None:
    parser = NDJSONEnvelopeParser()
    try:
        parser.parse_line("{not valid json}")
    except MalformedEnvelopeError as exc:
        assert "JSON decode failed" in str(exc)
    else:
        raise AssertionError("Expected parser to reject malformed JSON.")

