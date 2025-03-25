import functools

from common.base_logger import logger
from config.system_config import SystemSettingsConfig
from elements import (
    H265ParseWrapper,
    H264ParseWrapper,
    RTPH264Pay,
    CapsFilterWrapper,
    VideoAppSink,
    NVH264EncWrapper
)
from elements.gldownload import GLDownloadWrapper
from pipelines.mp2t_pipeline import MP2TStreamPipeline
from config.pipelines_config import CapsConfig

import gi
from gi.repository import Gst

gi.require_version('Gst', '1.0')

caps_conf = CapsConfig()
system_conf = SystemSettingsConfig()


class MP2TH265StreamPipeline(MP2TStreamPipeline):
    def __init__(self):
        super().__init__()
        self._elements = []
        initialized_pipeline_elements_tuple = (H265ParseWrapper("h265parser"),
                                               self.select_h265_decoder(system_conf.use_gpu),
                                               self.select_h264_encoder(system_conf.use_gpu),
                                               CapsFilterWrapper("h264capsfilter"),
                                               H264ParseWrapper("h264parser"),
                                               RTPH264Pay("rtph264pay"),
                                               VideoAppSink("videosink")
                                               )

        (self.h265parse, self.h265decoder, self.h264encoder,
         self.capsfilter, self.h264parse, self.rtph264pay, self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.h265parse, self.h265decoder, self.h264encoder, self.h264parse, self.rtph264pay,
                    self.videosink]

        self.has_elements_initialized(elements)

        self.h264encoder.set_property("bitrate", 4096)
        if isinstance(self.h264encoder, NVH264EncWrapper):
            self.h264encoder.set_property("preset", "low-latency-hp")
            self.h264encoder.set_property("zerolatency", True)
            self.h264encoder.set_property("gop-size", 30)
            self.h265decoder.set_property("max-errors", -1)
        else:
            self.h264encoder.set_property("key-int-max", 30)
            self.h264encoder.set_property("tune", "zerolatency")
            self.h264encoder.set_property("speed-preset", "ultrafast")

        self.h265parse.set_property("config-interval", -1)
        self.h264parse.set_property("config-interval", -1)
        self.rtph264pay.set_property("config-interval", -1)

        self.videosink.set_property("emit-signals", True)
        self.videosink.set_property("sync", False)
        self.videosink.set_property("async", False)

        self.capsfilter.set_property("caps", caps_conf.h264)

    def create_pipeline(self) -> Gst.Pipeline:
        self._add_elements()
        self._connect_signals()
        self._link_elements()
        return self._instance

    def _add_elements(self):
        elements_to_add = [
            self.h265parse, self.h265decoder, self.h264encoder, self.capsfilter, self.h264parse,
            self.rtph264pay, self.videosink
        ]
        self.add_elements(elements_to_add)

    def _connect_signals(self):
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

    def _link_elements(self):
        links = [
            (self.h265parse, self.h265decoder),
            (self.h265decoder, self.h264encoder),
            (self.h264encoder, self.capsfilter),
            (self.capsfilter, self.h264parse),
            (self.h264parse, self.rtph264pay),
            (self.rtph264pay, self.videosink),
        ]
        self.link_elements(links)
        pipeline_to_string = self.get_pipeline_string(links)
        logger.info("Pipeline string: %s", pipeline_to_string)

    def get_parser(self):
        return self.h265parse
