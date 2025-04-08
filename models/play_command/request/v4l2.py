from pydantic import Field
from typing_extensions import Literal, Optional
from .base_stream import BaseStreamModel, StreamType


class V4L2StreamModel(BaseStreamModel):
    stream_type: Literal[StreamType.V4L2]
    v4l2_src: Optional[str] = Field(None,description="v4l2 src file.")