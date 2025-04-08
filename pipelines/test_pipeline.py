import functools

from common.base_logger import logger
from config_models.system_config import SystemSettingsConfig
from elements import X264enc, H264ParseWrapper, RTPH264Pay
from elements.appsink import VideoAppSink
from elements.videotestsrc import VideoTestSrcWrapper
from pipelines import BaseStreamPipeline
from gi.repository import Gst


system_conf = SystemSettingsConfig()


class TESTStreamPipeline(BaseStreamPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (VideoTestSrcWrapper("videotestsrc"),
                                               self.select_h264_encoder(system_conf.use_gpu),
                                               H264ParseWrapper("h264parse"),
                                               RTPH264Pay("rtph264pay"),
                                               VideoAppSink("videosink")
                                               )

        (self.videotestsrc, self.h264encoder, self.h264parse, self.rtph264pay,
         self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.videotestsrc, self.h264encoder, self.h264parse, self.rtph264pay, self.videosink]

        self.has_elements_initialized(elements)

        if isinstance(self.h264encoder, X264enc):
            self.h264encoder.set_property("bitrate", 4096)
            self.h264encoder.set_property("key-int-max", 30)
            self.h264encoder.set_property("tune", "zerolatency")
            self.h264encoder.set_property("speed-preset", "ultrafast")

        self.videosink.set_property("emit-signals", True)
        self.videosink.set_property("async", False)

        self.h264parse.set_property("config-interval", -1)
        self.rtph264pay.set_property("config-interval", -1)

        self.rtph264pay.set_property("pt", 96)

        self.videotestsrc.set_property("pattern", 18)
    def create_pipeline(self) -> Gst.Pipeline:
        self._add_elements()
        self._connect_signals()
        self._link_elements()
        return self._instance

    def _add_elements(self):
        elements_to_add = [
            self.videotestsrc, self.h264encoder, self.h264parse, self.rtph264pay, self.videosink
        ]
        self.add_elements(elements_to_add)

    def _connect_signals(self):
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

    def _link_elements(self):
        links = [
            (self.videotestsrc, self.h264encoder),
            (self.h264encoder, self.h264parse),
            (self.h264parse, self.rtph264pay),
            (self.rtph264pay, self.videosink)
        ]
        self.link_elements(links)
        pipeline_to_string = self.get_pipeline_string(links)
        logger.info("Pipeline string: %s", pipeline_to_string)