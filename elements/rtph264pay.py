import functools

from common.base_logger import logger

from elements.h265parse import H265ParseWrapper
from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst, GLib, GstApp


class RTPH264Pay(GStreamerElementWrapper):
    def __init__(self, name="rtph264pay"):
        super().__init__(name, "rtph264pay")

    @staticmethod
    def on_pad_added(_, pad, element) -> None:
        print("On pad_added RTPH264Pay")
        pad_name = pad.get_name()
        webrtcbin = element
        sink_pad = webrtcbin.get_static_pad('sink')
        if not sink_pad.is_linked():
            result = pad.link(sink_pad)
            if result == Gst.PadLinkReturn.OK:
                logger.info(f"Linked pad {pad_name} to webrtcbin sink.")
            else:
                logger.error(f"Failed to link pad {pad_name} to webrtcbin sink.")
