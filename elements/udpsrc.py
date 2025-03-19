from collections import namedtuple

import gi
from typing_extensions import override

from app_instance import app
from elements.base_element_wrapper import GStreamerElementWrapper

gi.require_version('Gst', '1.0')
gi.require_version("GstApp", "1.0")


class UDPSrcWrapper(GStreamerElementWrapper):

    def __init__(self, name="udpsrc"):
        super().__init__("udpsrc", name)
        self._multicast_data = self.get_multicast_data()

    @staticmethod
    def get_multicast_data():
        if not hasattr(app.state, "request_data") or app.state.request_data is None:
            raise ValueError("request_data is not set in app.state yet.")
        return app.state.request_data.multicast_in

    def set_multicast_properties(self) -> None:
        AddressTuple = namedtuple("AddressTuple", ["ip", "port", "iface"])
        addr = AddressTuple(ip=self._multicast_data.ip, port=self._multicast_data.port, iface=self._multicast_data.nic)

        self.set_property("multicast-group", addr.ip)
        self.set_property("port", addr.port)
        # self.set_property("auto-multicast", True)
        self.set_property("multicast-iface", addr.iface)
