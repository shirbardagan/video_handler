from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst

class CapsFilterWrapper(GStreamerElementWrapper):
    def __init__(self, name="capsfilter"):
        super().__init__("capsfilter", name)

    def set_property(self, property_name: str, value) -> None:
        self._element.set_property(property_name, Gst.Caps.from_string(value))
        self._element_to_string += f" {property_name}={value} "

    # def link(self, other_element) -> None:
    #     try:
    #         identity_src_pad = self.get_element().get_static_pad("src")
    #         webrtcbin_dynamic_sink_pad = other_element.get_element().request_pad_simple("sink_%u")
    #         if identity_src_pad.link(webrtcbin_dynamic_sink_pad) == Gst.PadLinkReturn.OK:
    #             logger.info("Successfully linked %s to %s", self._name, other_element._name)
    #     except Exception as e:
    #         logger.error("While linking capsfilter with %s. %s", other_element._name, e)
