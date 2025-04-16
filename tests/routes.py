from fastapi.testclient import TestClient
from main import app


def test_enable_video_bit():
    with TestClient(app) as client:
        response = client.post("/", json={"command": "bit"})
        assert response.status_code == 200


def test_enable_video_keepalive():
    with TestClient(app) as client:
        response = client.post("/", json={"command": "keepalive"})
        res_json = response.json()

    assert response.status_code == 200
    assert res_json['status'] == True

    if __name__ == "__main__":
        test_enable_video_bit()
        test_enable_video_keepalive()
