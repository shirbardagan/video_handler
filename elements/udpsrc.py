from typing import override

import gi
from gi.repository import Gst, GLib, GstApp

from elements.base_element_wrapper import GStreamerElementWrapper

gi.require_version('Gst', '1.0')
gi.require_version("GstApp", "1.0")


class UDPSrcElementWrapper(GStreamerElementWrapper):
    @staticmethod
    def __init__(self, port, ip, name="source"):
        super().__init__(name, "source")

    @override
    def set_property(self, property_name: str, address_tuple):
        self.set_property("multicast-group", address_tuple.ip)
        self.set_property("port", address_tuple.port)
        self.set_property("auto-multicast", True)
