from typing import Union

from fastapi import APIRouter

from factory.stream_pipeline_factory import StreamPipelineFactory
from main import app
from models.mp2t import MP2TStreamModel
from models.mpeg4i import MPEG4IStreamConfig
from models.rtsp import RTSPStreamModel
from models.test import TestStreamModel
from models.udp import UDPStreamModel
from models.v4l2 import V4L2StreamModel

router = APIRouter()

StreamData = Union[
    RTSPStreamModel, UDPStreamModel, V4L2StreamModel, TestStreamModel, MPEG4IStreamConfig, MP2TStreamModel]
video_stream_factory = StreamPipelineFactory()

@router.post("/video/command/enable")
async def get_vmd_command(data: StreamData):
    app.state.PIPELINE_DATA = data
    pipeline = video_stream_factory.get_pipeline_type(data.stream_type)
    pipeline.start_pipeline(data)
