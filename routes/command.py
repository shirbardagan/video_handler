import socket

from fastapi import APIRouter, HTTPException, Request
from starlette.responses import JSONResponse
from typing_extensions import Union

import gi

gi.require_version('Gst', '1.0')
from gi.repository import Gst

from common.base_logger import logger
from config_models.config import PROPERTY_IFRAME_INTERVAL
from models.bit_keepalive import BitResponseModel, BitKeepAliveCommandsModel
from models.play_command.request.base_stream import StreamType

from app_instance import app
from factory.stream_pipeline_factory import StreamPipelineFactory
from models.play_command.request import *
from models.play_command.response import PlayResponseModel
from pipelines import BaseStreamPipeline

router = APIRouter()

StreamData = Union[
    RTSPStreamModel, RTPStreamModel, V4L2StreamModel, TestStreamModel, MPEG4IStreamConfig, MP2TStreamModel]

video_stream_factory = StreamPipelineFactory()


def get_host_ip_port() -> str:
    """Returns the local machine's IP and port."""
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        host_ip = s.getsockname()[0]
        return host_ip
    except socket.gaierror:
        logger.warning("Failed to retrieve local IP address, returning fallback 127.0.0.1.")
        return "127.0.0.1"


def generate_play_response(data: BaseStreamModel, server_port: int, success: bool = True, status_code: int = 200):
    """
    Generates a structured JSON response for video commands.

    Args:
        server_port: Server port.
        data (BaseStreamModel): Input data containing stream details.
        success (bool): Indicates if the operation was successful.
        status_code (int): HTTP status code.
    Returns:
        JSONResponse: A structured HTTP response.
    """
    logger.info(data)
    host_ip = get_host_ip_port()

    response_data = {
        "status": success,
        "ws_port": server_port,
        "host_ip": host_ip,
        # TODO: remove hardcoded endpoint and replace it with dynamic port
        "endpoint": f"ws://{host_ip}:{server_port}" if data.multicast_in else "",
        "active_ws_port": 8080,
        "klv": getattr(data, "klv", None)
    }
    return JSONResponse(content=PlayResponseModel(**response_data).dict(), status_code=status_code)


def generate_bit_response():
    data = app.state.request_data
    if data:
        multicast_settings = data.multicast_in
    else:
        multicast_settings = None
    curr_pipeline_gst_state = BaseStreamPipeline.check_pipeline_state()
    number_of_connections = len(app.state.conns)
    klv_settings = getattr(data, "klv", None)

    response_data = {
        "status": app.state.pipeline_status,
        "connected_users": number_of_connections,
        "state": curr_pipeline_gst_state,
        "bit": {
            "klv": klv_settings,
            "transcode": {
                "ip": multicast_settings.ip if multicast_settings else "",
                "port": multicast_settings.port if multicast_settings else 0,
                "nic": multicast_settings.nic if multicast_settings else "",
                "iframe_interval": PROPERTY_IFRAME_INTERVAL
            }
        }
    }
    return JSONResponse(content=BitResponseModel(**response_data).dict())


def generate_keepalive_response():
    curr_pipeline_gst_state = BaseStreamPipeline.check_pipeline_state()

    res = {
        "status": True,
        "stream":
            {
                "state": curr_pipeline_gst_state
            }
    }
    return res


@router.post("/")
async def enable_video(data: Union[StreamData, BitKeepAliveCommandsModel], request: Request):
    _, server_port = request.url.hostname, request.url.port
    if data.command == "play":
        app.state.request_data = data
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
            return generate_play_response(data, server_port, success=True, status_code=200)
        else:
            app.state.pipeline_status = False
            return generate_play_response(data, server_port, success=False, status_code=500)

    elif data.command == "bit":
        res = generate_bit_response()
        return res

    elif data.command == "keepalive":
        res = generate_keepalive_response()
        return res
