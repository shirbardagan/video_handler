import functools

from elements import (
    H264ParseWrapper,
    RTPH264Pay,
    AppSinkWrapper, VideoAppSink
)
from common.base_logger import logger
from pipelines.mp2t_pipeline import MP2TStreamPipeline

import gi
from gi.repository import Gst

gi.require_version('Gst', '1.0')


class MP2TH264StreamPipeline(MP2TStreamPipeline):
    def __init__(self):
        super().__init__()
        self._elements = []
        initialized_pipeline_elements_tuple = (H264ParseWrapper(),
                                               RTPH264Pay(),
                                               VideoAppSink()
                                               )

        (self.h264parse, self.rtph264pay, self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.h264parse, self.rtph264pay, self.videosink]

        if not all(elements):
            logger.error("Not all elements could be created.")

        self.videosink.set_property("emit-signals", True)

    def create_pipeline(self) -> Gst.Pipeline:
        self._add_elements()
        self._connect_signals()
        self._link_elements()
        return self._instance

    def _add_elements(self):
        elements_to_add = [
            self.h264parse, self.rtph264pay, self.videosink
        ]
        self.add_elements(elements_to_add)

    def _connect_signals(self):
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

    def _link_elements(self):
        links = [
            (self.h264parse, self.rtph264pay),
            (self.rtph264pay, self.videosink),
        ]
        self.link_elements(links)
        pipeline_to_string = self.get_pipeline_string(links)
        logger.info("Pipeline string: %s", pipeline_to_string)

    def get_parser(self):
        return self.h264parse
