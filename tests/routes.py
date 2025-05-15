from fastapi.testclient import TestClient
from main import app


def test_enable_video_bit():
    """Test the / endpoint with command='bit'."""

    with TestClient(app) as client:
        response = client.post("/", json={"command": "bit"})
        assert response.status_code == 200


def test_enable_video_keepalive():
    """Test the / endpoint with command='keepalive' and expect status=True in response."""

    with TestClient(app) as client:
        payload = {
            "command": "keepalive"
        }

        response = client.post("/", json=payload)
        res_json = response.json()

        assert response.status_code == 200
        assert res_json['status'] == True


def test_play_command_test_stream_type():
    """
    Test / endpoint with stream_type='test'.
    Expect a successful response with status True and ws_port = 8080 (default port).
    """

    with TestClient(app) as client:
        payload = {
            "command": "play",
            "stream_type": "test"
        }

        response = client.post("/", json=payload)
        assert response.status_code == 200
        json_data = response.json()
        assert json_data["status"] is True
        assert json_data["ws_port"] == 8080


def test_play_command_rtsp_stream_type():
    """
    Test / endpoint with stream_type='rtsp' and no rtsp_settings.
    Should return a 422 validation error.
    """

    with TestClient(app) as client:
        payload = {
            "command": "play",
            "stream_type": "rtsp"
        }

        response = client.post("/", json=payload)
        assert response.status_code == 422


def test_play_command_mp2t_stream_type_no_klv():
    """
    Test / endpoint with stream_type='mp2t' and no KLV provided.
    The validator should insert default KLV values, and the response should include them.
    """

    with TestClient(app) as client:
        payload = {
            "command": "play",
            "stream_type": "mp2t",
            "multicast_in": {"ip": "0.0.0.0",
                             "port": 8080,
                             "nic": "lo"}
        }

        response = client.post("/", json=payload)
        assert response.status_code == 200
        json_data = response.json()
        assert json_data["klv"]


def test_play_command_mp2t_stream_type_no_multicast_in():
    """
    Test / endpoint with stream_type='mp2t' but missing multicast_in.
    Should raise a 422 validation error.
    """

    with TestClient(app) as client:
        payload = {
            "command": "play",
            "stream_type": "mp2t"
        }

        response = client.post("/", json=payload)
        assert response.status_code == 422


def test_play_command_rtp_stream_type_no_multicast_in():
    """
    Test / endpoint with stream_type='rtp' but missing multicast_in.
    Should raise a 422 validation error.
    """

    with TestClient(app) as client:
        payload = {
            "command": "play",
            "stream_type": "rtp"
        }

        response = client.post("/", json=payload)
        assert response.status_code == 422


def test_enable_video_invalid_stream_type():
    with TestClient(app) as client:
        payload = {
            "command": "play",
            "stream_type": "invalid_stream_type"
        }
        response = client.post("/", json=payload)
        assert response.status_code == 422
