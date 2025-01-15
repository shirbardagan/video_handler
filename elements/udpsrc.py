import gi
from typing_extensions import override

from elements.base_element_wrapper import GStreamerElementWrapper

gi.require_version('Gst', '1.0')
gi.require_version("GstApp", "1.0")


class UDPSrcWrapper(GStreamerElementWrapper):
    @staticmethod
    def __init__(self, port, ip, type="udpsrc"):
        super().__init__(type, "udpsrc")

    @override
    def set_property(self, property_name: str, address_tuple) -> None:
        self.set_property("multicast-group", address_tuple.ip)
        self.set_property("port", address_tuple.port)
        self.set_property("auto-multicast", True)
