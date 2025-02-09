import json
from gi.repository import Gst, GLib, GstApp
import asyncio

from elements.base_element_wrapper import GStreamerElementWrapper


class WebRTCBinWrapper(GStreamerElementWrapper):
    allow_multiple_instances = True

    def __init__(self, type="webrtcbin"):
        super().__init__(type, "webrtcbin")

