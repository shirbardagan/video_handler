import functools
import gi
from gi.repository import Gst, GstRtsp
gi.require_version('Gst', '1.0')
gi.require_version('GstRtsp', '1.0')

from elements import (VideoAppSink,
                      UDPSrcWrapper,
                      RTPH264DePayWrapper,
                      H264ParseWrapper,
                      CapsFilterWrapper,
                      RTPH264Pay)
from pipelines import BaseStreamPipeline
from common.base_logger import logger
from config.pipelines_config import CapsConfig

caps_conf = CapsConfig()


class RTPStreamPipeline(BaseStreamPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (UDPSrcWrapper(),
                                               CapsFilterWrapper("capsfilter0"),
                                               RTPH264DePayWrapper(),
                                               H264ParseWrapper(),
                                               CapsFilterWrapper("capsfilter1"),
                                               RTPH264Pay(),
                                               VideoAppSink()
                                               )

        (self.udpsrc, self.capsfilter0, self.rtph264depay, self.h264parse, self.capsfilter1, self.rtph264pay,
         self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.udpsrc, self.rtph264depay, self.h264parse, self.capsfilter1, self.rtph264pay,
                    self.videosink]

        self.has_elements_initialized(elements)
        self.udpsrc.set_multicast_properties()

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
            self.udpsrc, self.capsfilter0, self.rtph264depay, self.h264parse, self.capsfilter1, self.rtph264pay,
            self.videosink
        ]
        self.add_elements(elements_to_add)

    def _connect_signals(self):
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

    def _link_elements(self):
        links = [
            (self.udpsrc, self.capsfilter0),
            (self.capsfilter0, self.rtph264depay),
            (self.rtph264depay, self.h264parse),
            (self.h264parse, self.capsfilter1),
            (self.capsfilter1, self.rtph264pay),
            (self.rtph264pay, self.videosink),
        ]
        self.link_elements(links)
        pipeline_to_string = self.get_pipeline_string(links)
        logger.info("Pipeline string: %s", pipeline_to_string)
