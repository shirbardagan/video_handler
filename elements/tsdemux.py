from app_instance import app
from common.base_logger import logger

from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst, GLib, GstApp

import threading

pad_lock = threading.Lock()


class TSDemuxWrapper(GStreamerElementWrapper):
    def __init__(self, name="tsdemux"):
        super().__init__("tsdemux", name)

    # @staticmethod
    # def on_pad_added(_, pad, elements) -> None:
    #     from factory.mp2t_encoded_pipeline import MP2TFactory
    #     print("On pad_added tsdemux.")
    #     pad_name = pad.get_name()
    #     pad_caps = pad.query_caps(None)
    #     structure_name = pad_caps.to_string()
    #     parser = elements[0]
    #     klvparse = elements[1]
    #     if 'video' in structure_name:
    #         # mp2t_pipeline = MP2TFactory().get_pipeline_type(structure_name.split(",")[0])
    #         # h264parse = mp2t_pipeline.h264parse
    #         sink_pad = parser.get_static_pad('sink')
    #         if not sink_pad.is_linked():
    #             result = pad.link(sink_pad)
    #             if result == Gst.PadLinkReturn.OK:
    #                 logger.info(f"Linked pad {pad_name} to parser sink.")
    #             else:
    #                 logger.error(f"While linking pad {pad_name} to parser sink.")
    #         app.state.CURR_PIPELINE.sync_children_states()
    #
    #     elif 'meta' in structure_name:
    #         sink_pad = klvparse.get_static_pad('sink')
    #         if not sink_pad.is_linked() and not pad_caps.is_empty():
    #             result = pad.link(sink_pad)
    #             if result == Gst.PadLinkReturn.OK:
    #                 logger.info(f"Linked pad {pad_name} to h265parse sink.")
    #             else:
    #                 logger.error(f"Failed to link pad {pad_name} to h265parse sink.")
    #     else:
    #         logger.warning(f"Unexpected pad: {pad_name} {structure_name}")

    @staticmethod
    def on_pad_added(klvparse, _, pad) -> None:
        from factory.mp2t_encoded_pipeline import MP2TFactory
        print("On pad_added tsdemux.")
        pad_name = pad.get_name()
        pad_caps = pad.query_caps(None)
        structure_name = pad_caps.to_string()
        if 'video' in structure_name:
            mp2t_pipeline = MP2TFactory().get_pipeline_type(structure_name.split(",")[0])
            mp2t_pipeline.create_pipeline()
            parser = mp2t_pipeline.get_parser()
            sink_pad = parser.get_element().get_static_pad('sink')
            if not sink_pad.is_linked():
                result = pad.link(sink_pad)
                if result == Gst.PadLinkReturn.OK:
                    logger.info(f"Linked pad {pad_name} to parser sink.")
                else:
                    logger.error(f"While linking pad {pad_name} to parser sink.")
            app.state.CURR_PIPELINE.sync_children_states()

        elif 'meta' in structure_name:
            sink_pad = klvparse.get_static_pad('sink')
            if not sink_pad.is_linked() and not pad_caps.is_empty():
                result = pad.link(sink_pad)
                if result == Gst.PadLinkReturn.OK:
                    logger.info(f"Linked pad {pad_name} to h265parse sink.")
                else:
                    logger.error(f"Failed to link pad {pad_name} to h265parse sink.")
        else:
            logger.warning(f"Unexpected pad: {pad_name} {structure_name}")