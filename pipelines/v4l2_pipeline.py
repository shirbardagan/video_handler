import functools

from app_instance import app
from config_models.system_config import SystemSettingsConfig
from elements import VideoConvertWrapper, CapsFilterWrapper, H264ParseWrapper, RTPH264Pay, VideoAppSink, V4L2SrcWrapper
from pipelines import BaseStreamPipeline
from config_models.config import CAPS_H264

import gi
from gi.repository import Gst

gi.require_version('Gst', '1.0')

system_conf = SystemSettingsConfig()


class V4L2StreamPipeline(BaseStreamPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (V4L2SrcWrapper("v4l2src"),
                                               VideoConvertWrapper(),
                                               self.select_h264_encoder(system_conf.use_gpu),
                                               CapsFilterWrapper("capsfilter"),
                                               H264ParseWrapper(),
                                               RTPH264Pay(),
                                               VideoAppSink()
                                               )

        (self.v4l2src, self.videoconvert, self.h264encoder, self.capsfilter, self.h264parse,
         self.rtph264pay, self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.v4l2src, self.videoconvert, self.h264encoder, self.capsfilter, self.h264parse,
                    self.rtph264pay, self.videosink]

        self.has_elements_initialized(elements)

        device_src = getattr(app.state.request_data, "v4l2_src")
        self.v4l2src.set_property("device", device_src)

        self.videosink.set_property("emit-signals", True)

        self.rtph264pay.set_property("config-interval", -1)

        self.capsfilter.set_property("caps", CAPS_H264)

    def create_pipeline(self) -> Gst.Pipeline:
        self._add_elements()
        self._connect_signals()
        self._link_elements()
        return self._instance

    def _add_elements(self):
        elements_to_add = [
            self.v4l2src, self.videoconvert, self.h264encoder, self.capsfilter, self.h264parse, self.rtph264pay,
            self.videosink
        ]
        self.add_elements(elements_to_add)

    def _connect_signals(self):
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

    def _link_elements(self):
        links = [
            (self.v4l2src, self.videoconvert),
            (self.videoconvert, self.h264encoder),
            (self.h264encoder, self.capsfilter),
            (self.capsfilter, self.h264parse),
            (self.h264parse, self.rtph264pay),
            (self.rtph264pay, self.videosink),
        ]
        self.link_elements(links)
