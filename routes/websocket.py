from fastapi import APIRouter, WebSocket

from starlette.websockets import WebSocketDisconnect

from app_instance import app
from common.base_logger import logger
from pipelines import RTSPStreamPipeline, MP2TH265StreamPipeline, UDPPipeline
from pipelines.mp2t_pipeline import MP2TStreamPipeline
from webrtc_handler.websocket_handler import WebRTCClient

import gi
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
    webrtc_client = WebRTCClient(conn)
    webrtc_client.start()
    try:
        if not hasattr(app.state, "CURR_PIPELINE") or app.state.CURR_PIPELINE is None:
            mpeg_pipe = MP2TStreamPipeline()
            mpeg_pipeline = mpeg_pipe.create_pipeline()


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
                print("In answer")
                answer = data["data"]
                sdp = answer["sdp"]

                answer_type = answer["type"]
                assert answer_type == "answer"

                res, sdpmsg = GstSdp.SDPMessage.new()
                GstSdp.sdp_message_parse_buffer(bytes(sdp.encode()), sdpmsg)
                answer = GstWebRTC.WebRTCSessionDescription.new(GstWebRTC.WebRTCSDPType.ANSWER, sdpmsg)
                promise = Gst.Promise.new()
                webrtc_client.webrtc.emit('set-remote-description', answer, promise)
                promise.interrupt()
                promise.wait()

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

    except WebSocketDisconnect:
        logger.info("WebSocket connection closed by the client.")
        webrtc_client.webrtc_pipeline.unref()
        app.state.CONN = None
    except Exception as e:
        logger.error("In websocket_endpoint: %s", e)
    finally:
        logger.info("WebSocket handler cleanup complete.")