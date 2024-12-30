from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst, GLib, GstApp

class TSDemuxWrapper(GStreamerElementWrapper):
    def __init__(self, name="tsdemux"):
        super().__init__(name, "tsdemux")

    def on_pad_added(self, demux, pad, elements) -> None:
        print("hhhhh")
        pad_name = pad.get_name()
        pad_caps = pad.query_caps(None)
        structure_name = pad_caps.to_string()
        h265parse = elements
        if 'video' in structure_name:
            sink_pad = h265parse.get_static_pad('sink')
            if not sink_pad.is_linked():
                result = pad.link(sink_pad)
                if result == Gst.PadLinkReturn.OK:
                    logger.info(f"Linked pad {pad_name} to h265parse sink.")
                else:
                    logger.error(f"Failed to link pad {pad_name} to h265parse sink.")
