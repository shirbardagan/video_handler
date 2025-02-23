import functools

from common.base_logger import logger
from elements import X264enc, H264ParseWrapper, RTPH264Pay
from elements.appsink import VideoAppSink
from elements.videotestsrc import VideoTestSrcWrapper
from pipelines.base_pipeline import BaseSinkPipeline
from gi.repository import Gst


class TESTStreamPipeline(BaseSinkPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (VideoTestSrcWrapper("videotestsrc"),
                                               X264enc("x264enc"),
                                               H264ParseWrapper("h264parse"),
                                               RTPH264Pay("rtph264pay"),
                                               VideoAppSink("videosink")
                                               )

        (self.videotestsrc, self.x264enc, self.h264parse, self.rtph264pay,
         self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.videotestsrc, self.x264enc, self.h264parse, self.rtph264pay, self.videosink]

        self.has_elements_initialized(elements)
        self.videosink.set_property("emit-signals", True)
        self.videosink.set_property("sync", False)
        self.rtph264pay.set_property("config-interval", -1)

    def create_pipeline(self) -> Gst.Pipeline:
        self._add_elements()
        self._connect_signals()
        self._link_elements()
        return self._instance

    def _add_elements(self):
        elements_to_add = [
            self.videotestsrc, self.x264enc, self.h264parse, self.rtph264pay, self.videosink
        ]
        self.add_elements(elements_to_add)

    def _connect_signals(self):
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

    def _link_elements(self):
        links = [
            (self.videotestsrc, self.x264enc),
            (self.x264enc, self.h264parse),
            (self.h264parse, self.rtph264pay),
            (self.rtph264pay, self.videosink)
        ]
        self.link_elements(links)
