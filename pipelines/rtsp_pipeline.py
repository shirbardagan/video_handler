import functools

from common.base_logger import logger
from elements import VideoAppSink, UDPSrcWrapper, RTPH264DePayWrapper, H264ParseWrapper, CapsFilterWrapper, RTPH264Pay, \
    RTSPSrcWrapper
from pipelines.base_pipeline import BaseStreamPipeline
from config.pipelines_config import CapsConfig

import gi

gi.require_version('Gst', '1.0')
gi.require_version('GstRtsp', '1.0')
from gi.repository import Gst, GstRtsp

from config.pipelines_config import CapsConfig

caps_conf = CapsConfig()


class RTSPStreamPipeline(BaseStreamPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (RTSPSrcWrapper("rtspsrc"),
                                               CapsFilterWrapper("capsfilter0"),
                                               RTPH264DePayWrapper(),
                                               H264ParseWrapper(),
                                               CapsFilterWrapper("capsfilter1"),
                                               RTPH264Pay(),
                                               VideoAppSink()
                                               )

        (self.rtspsrc, self.capsfilter0, self.rtph264depay, self.h264parse, self.capsfilter1, self.rtph264pay,
         self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.rtspsrc, self.rtph264depay, self.h264parse, self.capsfilter1, self.rtph264pay,
                    self.videosink]

        self.has_elements_initialized(elements)

        self.rtspsrc.set_property("location", "")


        self.capsfilter0.set_property("caps", caps_conf.rtp)
        self.capsfilter1.set_property("caps", caps_conf.h264)

        self.rtph264pay.set_property("config-interval", -1)

        self.videosink.set_property("emit-signals", True)
        self.videosink.set_property("sync", False)

    def create_pipeline(self) -> Gst.Pipeline:
        self._add_elements()
        self._connect_signals()
        self._link_elements()
        return self._instance

    def _add_elements(self):
        elements_to_add = [
            self.rtspsrc, self.capsfilter0, self.rtph264depay, self.h264parse, self.capsfilter1, self.rtph264pay,
            self.videosink
        ]
        self.add_elements(elements_to_add)

    def _connect_signals(self):
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

    def _link_elements(self):
        links = [
            (self.rtspsrc, self.capsfilter0),
            (self.capsfilter0, self.rtph264depay),
            (self.rtph264depay, self.h264parse),
            (self.h264parse, self.capsfilter1),
            (self.capsfilter1, self.rtph264pay),
            (self.rtph264pay, self.videosink),
        ]
        self.link_elements(links)
        pipeline_to_string = self.get_pipeline_string(links)
        logger.info("Pipeline string: %s", pipeline_to_string)
