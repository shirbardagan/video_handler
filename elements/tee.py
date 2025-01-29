from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst

class Tee(GStreamerElementWrapper):
    allow_multiple_instances = True

    def __init__(self, type="tee"):
        super().__init__(type, "tee")

    def link(self, other_element) -> None:
        try:
            identity_src_pad = self.get_element().get_static_pad("src")
            webrtcbin_dynamic_sink_pad = other_element.get_element().request_pad_simple("sink_%u")
            if identity_src_pad.link(webrtcbin_dynamic_sink_pad) == Gst.PadLinkReturn.OK:
                logger.info("Successfully linked %s to %s", self.get_name(), other_element.get_name())
        except Exception as e:
            logger.error("While linking tee with %s. %s", other_element.get_name(), e)
