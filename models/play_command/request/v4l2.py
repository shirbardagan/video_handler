from typing_extensions import Literal

from .base_stream import BaseStreamModel, StreamType


class V4L2StreamModel(BaseStreamModel):
    stream_type: Literal[StreamType.V4L2]
