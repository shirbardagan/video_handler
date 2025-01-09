from common.base_logger import logger
from elements import WebRTCBinWrapper
from elements.appsrc import AppSrcWrapper
from pipelines.base_pipeline import BaseSrcPipeline
from gi.repository import Gst, GObject, GstSdp


class WebRTCPipeline(BaseSrcPipeline):
    def __init__(self):
        self._instance = Gst.Pipeline.new("pipeline")
        initialized_pipeline_elements_tuple = (AppSrcWrapper("appsink"),
                                               WebRTCBinWrapper("webrtc")
                                               )

        (self.appsink, self.webrtc) = initialized_pipeline_elements_tuple

        elements = [self.appsink, self.webrtc]

        if not all(elements):
            logger.error("Not all elements could be created.")
