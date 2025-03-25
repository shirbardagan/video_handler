import socket

from fastapi import APIRouter, Body, HTTPException
from starlette.responses import JSONResponse
import gi
from gi.repository import Gst
from typing_extensions import Union

from common.base_logger import logger
from config.pipelines_config import ElementPropertiesConfig
from models.bit import BitResponseModel, GstStateEnum
from models.play_command.request.base_stream import StreamType

gi.require_version('Gst', '1.0')

from app_instance import app
from factory.stream_pipeline_factory import StreamPipelineFactory
from models.play_command.request import *
from models.play_command.response import PlayResponseModel

router = APIRouter()
element_properties_conf = ElementPropertiesConfig()

StreamData = Union[
    RTSPStreamModel, RTPStreamModel, V4L2StreamModel, TestStreamModel, MPEG4IStreamConfig, MP2TStreamModel]
video_stream_factory = StreamPipelineFactory()

def get_host_ip():
    """Returns the local machine's IP address."""
    try:
        return socket.gethostbyname(socket.gethostname())
    except socket.gaierror:
        return "127.0.0.1"

def check_pipeline_state():
    pipeline_state = getattr(app.state, "curr_pipeline", None)
    gst_state = None

    if pipeline_state:
        state = pipeline_state.get_state(Gst.CLOCK_TIME_NONE)[1]
        gst_state = GstStateEnum(state.value_nick.upper())
    return gst_state


def generate_play_response(data: StreamData, success: bool = True, status_code: int = 200):
    """
    Generates a structured JSON response for video commands.

    Args:
        data (BaseStreamModel): Input data containing stream details.
        success (bool): Indicates if the operation was successful.
        status_code (int): HTTP status code.
    Returns:
        JSONResponse: A structured HTTP response.
    """
    response_data = {
        "status": success,
        "ws_port": 8080,
        # "ws_port": data.multicast_in.port if data.multicast_in else 0,
        # "host_ip": data.multicast_in.ip if data.multicast_in else "",
        # TODO: remove hardcoded host-ip/port
        "host_ip": get_host_ip(),
        "endpoint": f"ws://0.0.0.0:8080" if data.multicast_in else "",
        "active_ws_port": 8080,
        # "active_ws_port": data.multicast_in.port if data.multicast_in else 0,
        "klv": getattr(data, "klv", None)
    }
    return JSONResponse(content=PlayResponseModel(**response_data).dict(), status_code=status_code)


def generate_bit_response():
    data = app.state.request_data
    pipeline_state = getattr(app.state, "curr_pipeline", None)
    gst_state = None

    if pipeline_state:
        state = pipeline_state.get_state(Gst.CLOCK_TIME_NONE)[1]
        gst_state = GstStateEnum(state.value_nick.upper())

    response_data = {
        "status": app.state.pipeline_status,
        "connected_users": len(app.state.conns),
        "state": gst_state,
        "bit": {
            "klv": getattr(data, "klv", None),
            "transcode": {
                "ip": data.multicast_in.ip if data.multicast_in else "",
                "port": data.multicast_in.port if data.multicast_in else 0,
                "nic": data.multicast_in.nic if data.multicast_in else "",
                "iframe_interval": element_properties_conf.iframe_interval
            }
        }
    }
    return JSONResponse(content=BitResponseModel(**response_data).dict())


@router.post("/")
async def enable_video(data: StreamData = Body(...)):
    app.state.request_data = data

    if data.command == "play":
        if data.stream_type not in StreamType.list():
            logger.error("Invalid stream type: %s. Allowed types: %s", data.stream_type, StreamType.list())
            raise HTTPException(status_code=400, detail=f"Invalid stream type: {data.stream_type}")

        if getattr(app.state, "curr_pipeline", None):
            app.state.curr_pipeline.set_state(Gst.State.NULL)

        pipeline = video_stream_factory.get_pipeline_type(data.stream_type)
        mpeg_pipeline = pipeline.create_pipeline()
        app.state.curr_pipeline = mpeg_pipeline

        if pipeline.start_pipeline():
            app.state.pipeline_status = True
            return generate_play_response(data, success=True, status_code=200)
        else:
            app.state.pipeline_status = False
            return generate_play_response(data, success=False, status_code=500)

    elif data.command == "bit":
            res = generate_bit_response()
            return res

    elif data.command == "keepalive":
        return {"status": True}