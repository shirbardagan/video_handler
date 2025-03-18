from fastapi import APIRouter, WebSocket

from starlette.websockets import WebSocketDisconnect

from app_instance import app
from common.base_logger import logger
from pipelines import RTSPStreamPipeline, RTPStreamPipeline
from pipelines.mp2t_pipeline import MP2TStreamPipeline
from pipelines.mpeg4i_pipeline import MPEG4IStreamPipeline
from pipelines.test_pipeline import TESTStreamPipeline
from pipelines.v4l2_pipeline import V4L2StreamPipeline
from webrtc_handler.websocket_handler import WebRTCClient
from config.system_config import SystemSettingsConfig

import gi

gi.require_version('Gst', '1.0')
from gi.repository import Gst

gi.require_version('GstWebRTC', '1.0')
from gi.repository import GstWebRTC

gi.require_version('GstSdp', '1.0')
from gi.repository import GstSdp

router = APIRouter()
system_conf = SystemSettingsConfig()


def handle_websocket_disconnect(webrtc_client: WebRTCClient):
    """Handles cleanup when the WebSocket connection is closed."""
    logger.info("WebSocket connection closed by the client.")

    if hasattr(webrtc_client, "videosrc") and webrtc_client.videosrc in app.state.webrtc_conn_videosrc:
        app.state.webrtc_conn_videosrc.remove(webrtc_client.videosrc)

    if hasattr(webrtc_client, "webrtc_pipeline"):
        webrtc_client.webrtc_pipeline.unref()
        webrtc_client.webrtc_pipeline = None

    if hasattr(webrtc_client, "conn") and webrtc_client.conn in app.state.conns:
        app.state.conns.remove(webrtc_client.conn)

    if hasattr(webrtc_client, "data_channel") and webrtc_client.data_channel in app.state.channels:
        app.state.channels.remove(webrtc_client.data_channel)

    logger.info("WebSocket handler cleanup complete.")


@router.websocket("/ws")
async def websocket_handler(conn: WebSocket):
    await conn.accept()
    app.state.conns.append(conn)
    if int(system_conf.max_users) > len(app.state.webrtc_conn_videosrc) or int(
            system_conf.max_users) == system_conf.default_value:
        webrtc_client = WebRTCClient(conn)
        webrtc_client.start()
        try:
            if not hasattr(app.state, "curr_pipeline") or app.state.curr_pipeline is None:
                mpeg_pipe = RTPStreamPipeline()
                mpeg_pipeline = mpeg_pipe.create_pipeline()

                ret = mpeg_pipeline.set_state(Gst.State.PLAYING)
                app.state.curr_pipeline = mpeg_pipeline
                if ret == Gst.StateChangeReturn.FAILURE:
                    logger.error("Unable to set the MPEGPipeline to the playing state")
                else:
                    logger.info("MPEGPipeline is now playing!!")

            while True:
                data = await conn.receive_json()
                event = data.get("event")

                if event == "answer":
                    await handle_answer(webrtc_client, data)

                elif event == "candidate":
                    await handle_candidate(webrtc_client, data)
        except WebSocketDisconnect:
            handle_websocket_disconnect(webrtc_client)
        except Exception as e:
            logger.error("In websocket endpoint: %s", e)
        finally:
            logger.info("WebSocket handler cleanup complete.")
    else:
        logger.warning("Reached to maximum %s connections. Not creating WebRTC connection.", system_conf.max_users)
        await conn.send_json(
            {"warning": f"Maximum {system_conf.max_users} connections reached. Not opening connection."})
        await conn.close()
        return


async def handle_answer(webrtc_client: WebRTCClient, data):
    """Handles the answer from WebRTC client."""
    answer = data["data"]
    sdp = answer["sdp"]
    answer_type = answer["type"]

    if answer_type != "answer":
        logger.error("Expected 'answer' type, got %s", answer_type)
        return

    res, sdpmsg = GstSdp.SDPMessage.new()
    GstSdp.sdp_message_parse_buffer(bytes(sdp.encode()), sdpmsg)
    answer = GstWebRTC.WebRTCSessionDescription.new(GstWebRTC.WebRTCSDPType.ANSWER, sdpmsg)
    promise = Gst.Promise.new()
    webrtc_client.webrtc.emit('set-remote-description', answer, promise)
    promise.interrupt()
    promise.wait()


async def handle_candidate(webrtc_client: WebRTCClient, data):
    """Handles the candidates from WebRTC client."""
    candidate = data["data"]
    candidate_val = candidate['candidate']
    webrtc_client.webrtc.emit('add-ice-candidate', candidate["sdpMLineIndex"], candidate_val)
