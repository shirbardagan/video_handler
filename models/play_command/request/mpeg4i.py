from typing_extensions import Literal

from .base_stream import BaseStreamModel, StreamType


class MPEG4IStreamConfig(BaseStreamModel):
    stream_type: Literal[StreamType.MPEG4I]
