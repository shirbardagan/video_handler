from fastapi import APIRouter, WebSocket, WebSocketDisconnect, Request
import gi

from app_instance import app
from common.base_logger import logger
from test_webrtc_pipeline import create_pipeline,play

gi.require_version('Gst', '1.0')
from gi.repository import Gst

gi.require_version('GstWebRTC', '1.0')
from gi.repository import GstWebRTC

gi.require_version('GstSdp', '1.0')
from gi.repository import GstSdp

router = APIRouter()


@router.websocket("/ws")
async def websocket_handler(conn: WebSocket):
    await conn.accept()

    app.state.CONN = conn
    try:
        pipeline = create_pipeline()
        play(pipeline)

        while True:
            data = await conn.receive_json()
            event = data.get("event")

            if event == "answer":
                answer = data["data"]
                await pipeline.handle_answer(answer)
            elif event == "candidate":
                candidate = data["data"]
                await pipeline.handle_candidate(candidate)
            elif event == "play":
                await pipeline.play()
            return conn
    except Exception as e:
        logger.error("In websocket_endpoint: %s", e)
