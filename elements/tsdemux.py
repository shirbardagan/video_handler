from common.base_logger import logger

from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst, GLib, GstApp

import threading

pad_lock = threading.Lock()
class TSDemuxWrapper(GStreamerElementWrapper):
    def __init__(self, type="tsdemux"):
        super().__init__(type, "tsdemux")

    @staticmethod
    def on_pad_added(_, pad, elements) -> None:
        from factory.mp2t_encoded_pipeline import MP2TFactory
        print("On pad_added tsdemux.")
        # with pad_lock:
        pad_name = pad.get_name()
        pad_caps = pad.query_caps(None)
        structure_name = pad_caps.to_string()
        if 'video' in structure_name:
            # mp2t_pipeline = MP2TFactory().get_pipeline_type(structure_name.split(",")[0])
            # h264parse = mp2t_pipeline.h264parse
            sink_pad = elements.get_static_pad('sink')
            if not sink_pad.is_linked():
                result = pad.link(sink_pad)
                if result == Gst.PadLinkReturn.OK:
                    logger.info(f"Linked pad {pad_name} to h265parse sink.")
                else:
                    logger.error(f"While linking pad {pad_name} to h265parse sink.")
