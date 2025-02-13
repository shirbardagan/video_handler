import functools

from common.base_logger import logger
from elements import RTSPSrcWrapper, VideoAppSink
from pipelines.base_pipeline import BaseSinkPipeline

import gi
from gi.repository import Gst

gi.require_version('Gst', '1.0')


class RTSPStreamPipeline(BaseSinkPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (RTSPSrcWrapper(),
                                               VideoAppSink()
                                               )

        (self.rtspsrc, self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.rtspsrc, self.videosink]

        super().has_elements_initialized(elements)

    def create_pipeline(self):
        try:
            self._instance.add(self.rtspsrc.get_element())
            self._instance.add(self.videosink.get_element())
        except Exception as e:
            logger.error("While adding elements to the pipeline: %s", e)
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

        self.rtspsrc.link(self.videosink)

        return self._instance

    def start_pipeline(self) -> None:
        pass

    def stop_pipeline(self) -> None:
        pass
