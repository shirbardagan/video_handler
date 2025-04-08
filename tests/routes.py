import pytest
from fastapi.testclient import TestClient
from app_instance import app
from models.bit_keepalive import BitKeepAliveCommandsModel
from models.play_command.request.base_stream import StreamType
from unittest.mock import patch

client = TestClient(app)


@pytest.fixture
def mock_pipeline():
    with patch('factory.stream_pipeline_factory.StreamPipelineFactory.get_pipeline_type') as mock_get_pipeline:
        yield mock_get_pipeline


@pytest.fixture
def valid_data():
    return {
        "command": "play",
        "stream_type": StreamType.RTSP.value,  # Adjust this based on the StreamType enum
    }


@pytest.fixture
def bit_data():
    return {
        "command": "bit",
    }


@pytest.fixture
def keepalive_data():
    return {
        "command": "keepalive",
    }


def test_enable_video_play(mock_pipeline, valid_data):
    mock_pipeline.return_value.create_pipeline.return_value.start_pipeline.return_value = True

    response = client.post("/", json=valid_data)
    assert response.status_code == 200
    response_data = response.json()
    assert response_data["status"] is True
    assert response_data["ws_port"] == 8080
    assert response_data["host_ip"] == "188.20.1.79"
    assert response_data["endpoint"] == "ws://0.0.0.0:8080"


def test_enable_video_invalid_stream_type(valid_data):
    valid_data["stream_type"] = "invalid_stream_type"
    response = client.post("/", json=valid_data)
    assert response.status_code == 400
    assert "Invalid stream type" in response.json()["detail"]


def test_enable_video_pipeline_start_failure(mock_pipeline, valid_data):
    # Mock the pipeline creation process to simulate failure
    mock_pipeline.return_value.create_pipeline.return_value.start_pipeline.return_value = False

    response = client.post("/", json=valid_data)
    assert response.status_code == 500
    assert response.json()["status"] is False


def test_generate_bit_response(bit_data):
    response = client.post("/", json=bit_data)
    assert response.status_code == 200
    response_data = response.json()
    assert response_data["status"] is True
    assert "bit" in response_data


def test_generate_keepalive_response(keepalive_data):
    response = client.post("/", json=keepalive_data)
    assert response.status_code == 200
    response_data = response.json()
    assert response_data["status"] is True
    assert "stream" in response_data
    assert "state" in response_data["stream"]


if __name__ == "__main__":
    pytest.main()
