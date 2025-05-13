from typing_extensions import Literal

from .base_stream import BaseStreamModel, StreamType


class TestStreamModel(BaseStreamModel):
    stream_type: Literal[StreamType.TEST]
