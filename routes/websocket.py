import time

from fastapi import APIRouter, WebSocket
import gi

from app_instance import app
from common.base_logger import logger
from pipelines.mp2t_h265_pipeline import MP2TH265StreamPipeline
from webrtc_handler.websocket_handler import WebRTCClient

gi.require_version('Gst', '1.0')
from gi.repository import Gst

gi.require_version('GstWebRTC', '1.0')
from gi.repository import GstWebRTC

gi.require_version('GstSdp', '1.0')
from gi.repository import GstSdp

router = APIRouter()


@router.websocket("/ws")
async def websocket_handler(conn: WebSocket):
    print("in /ws")
    await conn.accept()
    print("connection accepted")
    app.state.CONN = conn
    try:
        webrtc_client = WebRTCClient(conn)
        webrtc_client.start()
        while webrtc_client.webrtc.get_state(Gst.CLOCK_TIME_NONE)[1] != Gst.State.PLAYING:
            print("Not ready Yet")
        mpeg_pipe = MP2TH265StreamPipeline()
        mpeg_pipeline = mpeg_pipe.create_pipeline()

        filesrc = mpeg_pipeline.get_by_name("filesrc")
        filesrc.set_property("location", "/home/elbit/Desktop/flights/VNIR_ZOOM.ts")

        ret = mpeg_pipeline.set_state(Gst.State.PLAYING)
        app.state.CURR_PIPELINE = mpeg_pipeline
        if ret == Gst.StateChangeReturn.FAILURE:
            logger.error("Unable to set the MPEGPipeline to the playing state")
        else:
            logger.info("MPEGPipeline is now playing!!")
        while True:
            data = await conn.receive_json()
            event = data.get("event")

            if event == "answer":
                # webrtc_client.webrtc.get_static_pad("sink_0").remove_probe(1)
                print("In answer")
                answer = data["data"]
                sdp = answer["sdp"]

                answer_type = answer["type"]
                assert answer_type == "answer"

                res, sdpmsg = GstSdp.SDPMessage.new_from_text(sdp)
                GstSdp.sdp_message_parse_buffer(bytes(sdp.encode()), sdpmsg)
                answer = GstWebRTC.WebRTCSessionDescription.new(GstWebRTC.WebRTCSDPType.ANSWER, sdpmsg)
                promise = Gst.Promise.new()
                webrtc_client.webrtc.emit('set-remote-description', answer, promise)
                promise.interrupt()
                promise.wait()
                promise_result = promise.get_reply()
                webrtc_client.webrtc.get_static_pad("sink_0").remove_probe(1)
                print(f"Set remote description result: {promise_result}")
                print(webrtc_client.webrtc.get_static_pad("sink_0").is_blocked())
                print(webrtc_client.webrtc.get_static_pad("sink_0").is_blocking())
                webrtc_client.start()

            elif event == "candidate":
                print("In candidate")
                candidate = data["data"]
                candidate_val = candidate['candidate']
                webrtc_client.webrtc.emit('add-ice-candidate', candidate["sdpMLineIndex"], candidate_val)
                webrtc_client.start()

            elif event == "play":
                print("In play")
                webrtc_client.start()

    except Exception as e:
        logger.error("In websocket_endpoint: %s", e)
