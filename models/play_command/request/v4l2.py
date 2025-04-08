from pydantic import Field
from typing_extensions import Literal
from .base_stream import BaseStreamModel, StreamType


class V4L2StreamModel(BaseStreamModel):
    stream_type: Literal[StreamType.V4L2]
    v4l2_src: str = Field(description="v4l2 src file.")