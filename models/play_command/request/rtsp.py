from typing_extensions import Literal

from .base_stream import BaseStreamModel, StreamType


class RTSPStreamModel(BaseStreamModel):
    stream_type: Literal[StreamType.RTSP]
