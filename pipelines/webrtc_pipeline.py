from common.base_logger import logger
from elements import WebRTCBinWrapper
from elements.appsrc import VideoAppSrc, DataAppSrc
from pipelines.base_pipeline import BaseSrcPipeline

import gi
gi.require_version("GstSdp", "1.0")
from gi.repository import Gst, GObject, GstSdp


class WebRTCPipeline(BaseSrcPipeline):
    def __init__(self):
        super().__init__()
        self._instance = Gst.Pipeline.new("pipeline")
        initialized_pipeline_elements_tuple = (VideoAppSrc(),
                                               DataAppSrc(),
                                               WebRTCBinWrapper()
                                               )

        (self.videosrc, self.datasrc, self.webrtcbin) = initialized_pipeline_elements_tuple

        elements = [self.videosrc, self.datasrc, self.webrtcbin]

        if not all(elements):
            logger.error("Not all elements could be created.")
        self.videosrc.connect("need-data", self.videosrc.on_need_data)

        
    def create_pipeline(self):
        self._instance.add(self.videosrc.get_element())
        self._instance.add(self.datasrc.get_element())
        self._instance.add(self.webrtcbin.get_element())

        self.videosrc.link(self.webrtcbin)
        return self._instance

