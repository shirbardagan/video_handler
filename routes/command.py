from fastapi import APIRouter, Body
from starlette.responses import JSONResponse

from app_instance import app
from common.base_logger import logger
from factory.stream_pipeline_factory import StreamPipelineFactory
from models.play_command.request import *

import gi

from models.play_command.response import PlayResponseModel

gi.require_version('Gst', '1.0')
from gi.repository import Gst

router = APIRouter()

video_stream_factory = StreamPipelineFactory()

def generate_response(data):
    response_data = {"status": True, "ws_port": data.multicast_in.port, "host_ip": data.multicast_in.ip,
                     "endpoint": f"{data.multicast_in.ip}:{str(data.multicast_in.port)}",
                     "active_ws_port": data.multicast_in.port, "klv": None}
    response_data = PlayResponseModel(**response_data)
    return response_data

@router.post("/video/command/enable")
async def enable_video(data: BaseStreamModel = Body(...)):
    if data.command == "play":
        app.state.request_data = data

        # if hasattr(app.state, "curr_pipeline"):
        #     if app.state.curr_pipeline is not None:
        #         app.state.curr_pipeline.set_state(Gst.State.NULL)
        #
        # if hasattr(app.state, "curr_object"):
        #     if app.state.curr_object is not None:
        #         app.state.curr_object.unref()

        pipeline = video_stream_factory.get_pipeline_type(data.stream_type)
        app.state.curr_object = pipeline

        mpeg_pipeline = pipeline.create_pipeline()
        app.state.curr_pipeline = mpeg_pipeline

        pipeline_status = pipeline.start_pipeline()

        if pipeline_status:
            res = generate_response(data)
            return res
    elif data.command == "bit":
        pass
