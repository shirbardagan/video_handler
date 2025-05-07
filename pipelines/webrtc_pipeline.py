import time

from app_instance import app
from common.base_logger import logger
from elements import WebRTCBinWrapper
from elements.appsrc import VideoAppSrc
from pipelines import BaseStreamPipeline

import gi

gi.require_version("GstSdp", "1.0")
from gi.repository import Gst, GObject, GstSdp


class WebRTCPipeline(BaseStreamPipeline):
    def __init__(self):
        try:
            super().__init__()
            initialized_pipeline_elements_tuple = (VideoAppSrc("videosrc"),
                                                   WebRTCBinWrapper("webrtcbin")
                                                   )

            (self.videosrc, self.webrtcbin) = initialized_pipeline_elements_tuple

            elements = [self.videosrc, self.webrtcbin]
            # self.webrtcbin.connect("pad-added", self.on_pad_added)
            if not all(elements):
                logger.error("Not all elements could be created.")

            self.videosrc.connect("need-data", self.videosrc.on_need_data)

            self.videosrc.set_property("format", Gst.Format.TIME)
            self.videosrc.set_property("is-live", True)
            self.videosrc.set_property("do-timestamp", True)
        except Exception as e:
            logger.error("While initializing WebRTCbin pipeline: %s", e)

    # def on_pad_added(self, _, pad):
    #     pad.add_probe(Gst.PadProbeType.BUFFER, self.videosrc_probe)

    # app.state.counter2 = 0
    # app.state.av_arr = []
    # app.state.time_inside1 = 0
    # def videosrc_probe(self, pad, info):
    #     buffer = info.get_buffer()
    #     print("weeeeebbbbrrtcc",app.state.time_inside1)
    #     app.state.time_inside1+=1
    #     if buffer:
    #         meta = app.state.buffer_meta.get(buffer.pts)
    #         if meta:
    #             m = meta["custom_id"]
    #             print(m)
    #     return Gst.PadProbeReturn.OK

    def create_pipeline(self) -> Gst.Pipeline:
        self._add_elements()
        self._link_elements()
        return self._instance

    def _add_elements(self):
        elements_to_add = [self.videosrc, self.webrtcbin]
        self.add_elements(elements_to_add)

    def _link_elements(self):
        links = [(self.videosrc, self.webrtcbin)]
        self.link_elements(links)
        pipeline_to_string = self.get_pipeline_string(links)
        logger.info("Pipeline string: %s", pipeline_to_string)
