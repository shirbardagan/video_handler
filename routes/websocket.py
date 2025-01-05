from fastapi import APIRouter, WebSocket, WebSocketDisconnect, Request
import gi

from app_instance import app
from common.base_logger import logger
from test_webrtc_pipeline import create_pipeline,play
from webrtc_handler.websocket_handler import WebRTCClient

gi.require_version('Gst', '1.0')
from gi.repository import Gst

gi.require_version('GstWebRTC', '1.0')
from gi.repository import GstWebRTC

gi.require_version('GstSdp', '1.0')
from gi.repository import GstSdp
from threading import Thread

router = APIRouter()


@router.websocket("/ws")
async def websocket_handler(conn: WebSocket):
    print("in /ws")
    await conn.accept()
    print("connection accepted")
    app.state.CONN = conn
    try:
        pipeline = WebRTCClient(conn)
        pipeline.start()
        print("before while loop")
        while True:
            print("in while loop")
            data = await conn.receive_json()
            event = data.get("event")

            if event == "answer":
                answer = data["data"]
                sdp = answer["sdp"]

                answer_type = answer["type"]
                assert answer_type == "answer"

                res, sdpmsg = GstSdp.SDPMessage.new()
                GstSdp.sdp_message_parse_buffer(bytes(sdp.encode()), sdpmsg)
                answer = GstWebRTC.WebRTCSessionDescription.new(GstWebRTC.WebRTCSDPType.ANSWER, sdpmsg)
                promise = Gst.Promise.new()
                pipeline.webrtc.get_element().emit('set-remote-description', answer, promise)
                promise.interrupt()
                pipeline.play()

            elif event == "candidate":
                candidate = data["data"]
                candidate_val = candidate['candidate']
                pipeline.webrtc.get_element().emit('add-ice-candidate', candidate["sdpMLineIndex"], candidate_val)
                pipeline.play()

            elif event == "play":
                pipeline.play()

    except Exception as e:
        logger.error("In websocket_endpoint: %s", e)
