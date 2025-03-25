from typing_extensions import Literal

from .base_stream import BaseStreamModel, StreamType


class RTPStreamModel(BaseStreamModel):
    stream_type: Literal[StreamType.RTP]