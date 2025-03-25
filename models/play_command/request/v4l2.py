from typing import Union, List

from .base_stream import BaseStreamModel


class V4L2StreamModel(BaseStreamModel):
    stream_type: str = "v4ls"
