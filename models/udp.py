from models.base_stream import BaseStreamModel

class UDPStreamModel(BaseStreamModel):
    stream_type: str = "udp"