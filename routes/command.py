from typing_extensions import Union

from fastapi import APIRouter, Body

from app_instance import app
from common.base_logger import logger
from factory.stream_pipeline_factory import StreamPipelineFactory
from models.play_command.request import *

import gi

from models.play_command.request.base_stream import BaseStreamModel

gi.require_version('Gst', '1.0')
from gi.repository import Gst

router = APIRouter()

StreamData = Union[
    RTPStreamModel, RTSPStreamModel, V4L2StreamModel, TestStreamModel, MPEG4IStreamConfig, MP2TStreamModel]
video_stream_factory = StreamPipelineFactory()


@router.post("/video/command/enable")
async def enable_video(data: BaseStreamModel = Body(...)):
    if data.command == "play":
        if hasattr(app.state, "curr_pipeline"):
            if app.state.curr_pipeline is not None:
                app.state.curr_pipeline.set_state(Gst.State.NULL)
        print(data)
        app.state.request_data = data
        pipeline = video_stream_factory.get_pipeline_type(data.stream_type)
        if hasattr(app.state, "curr_object"):
            if app.state.curr_object is not None:
                app.state.curr_object.unref()
        app.state.curr_object = pipeline

        print(type(pipeline))
        mpeg_pipeline = pipeline.create_pipeline()

        ret = mpeg_pipeline.set_state(Gst.State.PLAYING)
        app.state.curr_pipeline = mpeg_pipeline
        if ret == Gst.StateChangeReturn.FAILURE:
            logger.error("Unable to set the MPEGPipeline to the playing state")
        else:
            logger.info("MPEGPipeline is now playing!!")

    elif data.command == "bit":
        pass
