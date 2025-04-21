from collections import namedtuple

from app_instance import app
from elements import GStreamerElementWrapper


class UDPSrcWrapper(GStreamerElementWrapper):

    def __init__(self, name="udpsrc"):
        super().__init__("udpsrc", name)
        self._multicast_data = self.get_multicast_data()

    @staticmethod
    def get_multicast_data():
        if hasattr(app.state, "request_data") or app.state.request_data is not None:
            return app.state.request_data.multicast_in

    def set_multicast_properties(self) -> None:
        AddressTuple = namedtuple("AddressTuple", ["ip", "port", "iface"])
        addr = AddressTuple(ip=self._multicast_data.ip, port=self._multicast_data.port, iface=self._multicast_data.nic)

        self.set_property("address", addr.ip)
        self.set_property("port", addr.port)
        self.set_property("multicast-iface", addr.iface)
