import gi

gi.require_version('Gst', '1.0')
gi.require_version("GstApp", "1.0")
from gi.repository import Gst, GLib, GstApp

from app_instance import app
from common.base_logger import logger
from elements import GStreamerElementWrapper


class TSDemuxWrapper(GStreamerElementWrapper):
    def __init__(self, name="tsdemux"):
        super().__init__("tsdemux", name)

    @staticmethod
    def on_pad_added(dataparser, _, pad) -> None:
        from factory.mp2t_encoded_pipeline import MP2TFactory
        logger.info("On on_pad_added of tsdemux")
        pad_name = pad.get_name()
        pad_caps = pad.query_caps(None)
        structure_name = pad_caps.to_string()
        logger.debug("Demuxing caps: %s.", structure_name)
        if 'video' in structure_name:
            mp2t_pipeline = MP2TFactory().get_pipeline_type(structure_name.split(",")[0])
            mp2t_pipeline.create_pipeline()
            parser = mp2t_pipeline.get_parser()
            sink_pad = parser.get_element().get_static_pad('sink')
            if not sink_pad.is_linked():
                result = pad.link(sink_pad)
                if result == Gst.PadLinkReturn.OK:
                    logger.info(f"Linked pad %s to %s sink.", pad_name, parser.get_name())
                else:
                    logger.error(f"While linking pad %s to %s sink.", pad_name, parser.get_name())
            app.state.curr_pipeline.sync_children_states()

        elif 'meta' in structure_name:
            sink_pad = dataparser.get_static_pad('sink')
            if not sink_pad.is_linked() and not pad_caps.is_empty():
                result = pad.link(sink_pad)
                if result == Gst.PadLinkReturn.OK:
                    logger.debug(f"Linked pad %s to klvparse sink.", pad_name)
                else:
                    logger.error(f"Failed to link pad %s to klvparse sink.", pad_name)
        else:
            logger.warning(f"Unexpected pad: %s - %s", pad_name, structure_name)
