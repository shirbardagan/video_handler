from fastapi import APIRouter, Body, HTTPException
from starlette.responses import JSONResponse
import gi
from gi.repository import Gst
from typing_extensions import Union

from common.base_logger import logger
from models.play_command.request.base_stream import StreamType

gi.require_version('Gst', '1.0')

from app_instance import app
from factory.stream_pipeline_factory import StreamPipelineFactory
from models.play_command.request import *
from models.play_command.response import PlayResponseModel

router = APIRouter()

StreamData = Union[
    RTSPStreamModel, RTPStreamModel, V4L2StreamModel, TestStreamModel, MPEG4IStreamConfig, MP2TStreamModel]
video_stream_factory = StreamPipelineFactory()


def generate_response(data: StreamData, success: bool = True, status_code: int = 200):
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
        "ws_port": data.multicast_in.port if data.multicast_in else 0,
        "host_ip": data.multicast_in.ip if data.multicast_in else "",
        "endpoint": f"ws://{data.multicast_in.ip}:{data.multicast_in.port}" if data.multicast_in else "",
        "active_ws_port": data.multicast_in.port if data.multicast_in else 0,
        "klv": getattr(data, "klv", None)
    }
    return JSONResponse(content=PlayResponseModel(**response_data).dict(), status_code=status_code)


@router.post("/video/command/enable")
async def enable_video(data: StreamData = Body(...)):
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
            return generate_response(data, success=True, status_code=200)
        else:
            return generate_response(data, success=False, status_code=500)

    elif data.command == "bit":
            pass
