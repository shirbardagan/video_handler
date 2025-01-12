import functools

from common.base_logger import logger

from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst, GLib, GstApp


class TSDemuxWrapper(GStreamerElementWrapper):
    def __init__(self, name="tsdemux"):
        super().__init__(name, "tsdemux")

    @staticmethod
    def on_pad_added(_, pad, elements) -> None:
        from factory.mp2t_encoded_pipeline import MP2TFactory
        print("On pad_added tsdemux.")
        pad_name = pad.get_name()
        pad_caps = pad.query_caps(None)
        structure_name = pad_caps.to_string()
        if 'video' in structure_name:
            mp2t_pipeline = MP2TFactory().get_pipeline_type(structure_name.split(",")[0])
            h265parse = mp2t_pipeline.h265parse
            sink_pad = h265parse.get_element().get_static_pad('sink')
            if not sink_pad.is_linked():
                result = pad.link(sink_pad)
                if result == Gst.PadLinkReturn.OK:
                    logger.info(f"Linked pad {pad_name} to h265parse sink.")
                else:
                    logger.error(f"While linking pad {pad_name} to h265parse sink.")
