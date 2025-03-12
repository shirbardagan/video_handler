import functools

from common.base_logger import logger

from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst, GLib, GstApp


class QTDemuxWrapper(GStreamerElementWrapper):
    def __init__(self, name="qtdemux"):
        super().__init__("qtdemux", name)

    @staticmethod
    def on_pad_added(_, pad, elements) -> None:
        print("On pad_added qtdemux.")
        pad_name = pad.get_name()
        pad_caps = pad.query_caps(None)
        structure_name = pad_caps.to_string()
        h264parse = elements
        sink_pad = h264parse.get_static_pad('sink')
        if 'video' in structure_name:
            if not sink_pad.is_linked():
                result = pad.link(sink_pad)
                if result == Gst.PadLinkReturn.OK:
                    logger.info(f"Linked pad %s to h265parse sink.", pad_name)
                else:
                    logger.error(f"While linking pad %s to h264parse sink.",pad_name)
