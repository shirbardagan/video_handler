from elements import (
    FileSrcWrapper,
    TSDemuxWrapper,
    H264ParseWrapper,
    RTPH264Pay,
    AppSinkWrapper
)
from common.base_logger import logger
from pipelines.mp2t_pipeline import MP2TStreamPipeline


class MP2TH264StreamPipeline(MP2TStreamPipeline):
    def __init__(self):
        super().__init__()
        self._elements = []
        initialized_pipeline_elements_tuple = (H264ParseWrapper(),
                                               RTPH264Pay(),
                                               AppSinkWrapper()
                                               )

        (
        self.h264parse, self.rtph264pay, self.appsink) = initialized_pipeline_elements_tuple

        elements = [self.filesrc, self.tsdemux, self.h264parse, self.rtph264pay, self.appsink]

        if not all(elements):
            logger.error("Not all elements could be created.")

        # self.filesrc.set_property("location", "/home/elbit/Desktop/flights/VNIR_ZOOM.ts")
        self.appsink.set_property("emit-signals", True)
