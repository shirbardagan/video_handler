from .base_stream import BaseStreamModel

class RTPStreamModel(BaseStreamModel):
    stream_type: str = "rtp"