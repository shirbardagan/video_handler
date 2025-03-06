from typing import Union, List

from .base_stream import BaseStreamModel


class V4L2StreamModel(BaseStreamModel):
    stream_type: str = "v4ls"
    use_jpeg: bool
    height: int
    width: int
    resolution: Union[str, List[int]]
    framerate: Union[str, int, List[int]]
