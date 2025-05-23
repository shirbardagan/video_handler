import gi

gi.require_version('Gst', '1.0')
from gi.repository import Gst

from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper


class RTPGSTPayWrapper(GStreamerElementWrapper):
    def __init__(self, name="rtpgstpay"):
        super().__init__(name, "rtpgstpay")

    def link(self, other_element) -> None:
        try:
            rtph264pay_src_pad = self.get_element().get_static_pad("src")
            webrtcbin_dynamic_sink_pad = other_element.get_element().request_pad_simple("sink_%u")
            if rtph264pay_src_pad.link(webrtcbin_dynamic_sink_pad) == Gst.PadLinkReturn.OK:
                logger.info("Successfully linked %s to %s", self._name, other_element._name)
        except Exception as e:
            logger.error("While linking identity with %s. %s", other_element._name, e)
