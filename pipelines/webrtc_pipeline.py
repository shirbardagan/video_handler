from common.base_logger import logger
from elements import WebRTCBinWrapper, CapsFilterWrapper
from elements.appsrc import VideoAppSrc, DataAppSrc
from elements.identity import Identity
from pipelines.base_pipeline import BaseSrcPipeline

import gi

gi.require_version("GstSdp", "1.0")
from gi.repository import Gst, GObject, GstSdp


class WebRTCPipeline(BaseSrcPipeline):
    def __init__(self):
        try:
            super().__init__()
            self._instance = Gst.Pipeline.new("pipeline")
            initialized_pipeline_elements_tuple = (VideoAppSrc(),
                                                   # Identity(),
                                                   # CapsFilterWrapper(),
                                                   # DataAppSrc(),
                                                   WebRTCBinWrapper()
                                                   )

            (self.videosrc, self.webrtcbin) = initialized_pipeline_elements_tuple

            elements = [self.videosrc,self.webrtcbin]

            if not all(elements):
                logger.error("Not all elements could be created.")

            # video_caps = Gst.Caps.from_string(
            #     "application/x-rtp, media=(string)video, encoding-name=(string)H264, payload=(int)96"
            # )
            self.videosrc.connect("need-data", self.videosrc.on_need_data)
            self.videosrc.connect("enough-data", self.videosrc.on_enough_data)
            self.videosrc.set_property("format", Gst.Format.TIME)
            self.videosrc.set_property("is-live", True)
            self.videosrc.set_property("do-timestamp", True)
            # self.videosrc.set_property("leaky-type", 2)
            # self.identity.set_property("dump", True)
            # self.capsfilter.set_property("caps", video_caps)

        except Exception as e:
            logger.error("While initializing webrtcbin pipeline: %s", e)

    def create_pipeline(self):
        self._instance.add(self.videosrc.get_element())
        # self._instance.add(self.datasrc.get_element())
        # self._instance.add(self.identity.get_element())
        self._instance.add(self.webrtcbin.get_element())

        self.videosrc.link(self.webrtcbin)
        # self.identity.link(self.webrtcbin)
        return self._instance
