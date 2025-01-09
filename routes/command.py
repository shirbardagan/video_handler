from typing import Union

from fastapi import APIRouter, Request, Body

from factory.stream_pipeline_factory import StreamPipelineFactory
from models import *

router = APIRouter()

StreamData = Union[
    RTSPStreamModel, UDPStreamModel, V4L2StreamModel, TestStreamModel, MPEG4IStreamConfig, MP2TStreamModel]
video_stream_factory = StreamPipelineFactory()


@router.post("/video/command/enable")
async def get_vmd_command(request: Request, data: StreamData = Body(...)):
    print("Aaaaaaaaa")
    print(data)
    request.app.state.PIPELINE_DATA = data
    pipeline = video_stream_factory.get_pipeline_type(data.stream_type)
    # pipeline.start_pipeline(data)
    print(type(pipeline))


