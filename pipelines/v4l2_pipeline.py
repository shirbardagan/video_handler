import functools

from common.base_logger import logger
from elements import VideoConvertWrapper, CapsFilterWrapper, H264ParseWrapper, RTPH264Pay, VideoAppSink, V4L2SrcWrapper
from elements.nvh264enc import NVH264EncWrapper
from pipelines.base_pipeline import BaseStreamPipeline
from config.pipelines_config import CapsConfig

import gi
from gi.repository import Gst

gi.require_version('Gst', '1.0')

caps_conf = CapsConfig()


class V4L2StreamPipeline(BaseStreamPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (V4L2SrcWrapper("v4l2src"),
                                               VideoConvertWrapper(),
                                               NVH264EncWrapper(),
                                               CapsFilterWrapper("capsfilter"),
                                               H264ParseWrapper(),
                                               RTPH264Pay(),
                                               VideoAppSink()
                                               )

        (self.v4l2src, self.videoconvert, self.nvh264enc, self.capsfilter, self.h264parse,
         self.rtph264pay, self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.v4l2src, self.videoconvert, self.nvh264enc, self.capsfilter, self.h264parse,
                    self.rtph264pay, self.videosink]

        self.has_elements_initialized(elements)
        self.v4l2src.set_property("device", "/dev/video0")
        self.videosink.set_property("emit-signals", True)

        self.rtph264pay.set_property("config-interval", -1)

        self.capsfilter.set_property("caps", caps_conf.h264)

    def create_pipeline(self) -> Gst.Pipeline:
        self._add_elements()
        self._connect_signals()
        self._link_elements()
        return self._instance

    def _add_elements(self):
        elements_to_add = [
            self.v4l2src, self.videoconvert, self.nvh264enc, self.capsfilter, self.h264parse, self.rtph264pay, self.videosink
        ]
        self.add_elements(elements_to_add)

    def _connect_signals(self):
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

    def _link_elements(self):
        links = [
            (self.v4l2src, self.videoconvert),
            (self.videoconvert, self.nvh264enc),
            (self.nvh264enc, self.capsfilter),
            (self.capsfilter, self.h264parse),
            (self.h264parse, self.rtph264pay),
            (self.rtph264pay, self.videosink),
        ]
        self.link_elements(links)