import asyncio
from test_webrtc_pipeline import pipeline

class WebRTCClient:
    def __init__(self, conn):
        self.conn = conn
        self.loop = asyncio.get_running_loop()

        self.pipeline = pipeline
