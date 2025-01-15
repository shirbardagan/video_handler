import json
from gi.repository import Gst, GLib, GstApp
import asyncio

from app_instance import app
from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper


class WebRTCBinWrapper(GStreamerElementWrapper):
    def __init__(self, type="webrtcbin"):
        super().__init__(type, "webrtcbin")

