import gi

gi.require_version('Gst', '1.0')
from gi.repository import Gst

from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper


class RTSPSrcWrapper(GStreamerElementWrapper):
    def __init__(self, name="rtspsrc"):
        super().__init__("rtspsrc", name)

    @staticmethod
    def on_pad_added(src, new_pad, target_element) -> bool:
        sink_pad = target_element.get_element().get_static_pad("sink")
        if sink_pad:
            result = new_pad.link(sink_pad)
            if result == Gst.PadLinkReturn.OK:
                logger.info("Successfully linked pad.")
            else:
                logger.error("Failed to link pad.")
                return False
        else:
            logger.error("Target sink pad not found.")
            return False
