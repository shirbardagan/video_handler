from models.base_stream import BaseStreamModel


class TestStreamModel(BaseStreamModel):
    stream_type: str = "test"