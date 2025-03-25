from .base_stream import BaseStreamModel


class RTSPStreamModel(BaseStreamModel):
    stream_type: str = "rtsp"
