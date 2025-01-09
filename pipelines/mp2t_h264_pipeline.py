from pipelines.base_pipeline import BaseStreamPipeline

from elements import (
    FileSrcWrapper,
    TSDemuxWrapper,
    H265ParseWrapper,
    NVH265DecWrapper,
    H264ParseWrapper,
    WebRTCBinWrapper,
    RTPH264Pay,
    X264enc, AppSinkWrapper
)
from common.base_logger import logger
from gi.repository import Gst, GObject, GstSdp

from pipelines.mp2t_pipeline import MP2TStreamPipeline


class MP2TH264StreamPipeline(MP2TStreamPipeline):
    def __init__(self):
        self._instance = Gst.Pipeline.new("pipeline")
        initialized_pipeline_elements_tuple = (FileSrcWrapper("filesrc"),
                                               TSDemuxWrapper("tsdemux"),
                                               H264ParseWrapper("h264parse"),
                                               RTPH264Pay("rtph264pay"),
                                               AppSinkWrapper("appsink")
                                               )

        (self.filesrc, self.tsdemux, self.h264parse, self.rtph264pay,
         self.appsink) = initialized_pipeline_elements_tuple

        elements = [self.filesrc, self.tsdemux, self.h264parse, self.rtph264pay, self.appsink]

        if not all(elements):
            logger.error("Not all elements could be created.")

        self.filesrc.set_property("location", "/home/elbit/Desktop/flights/VNIR_ZOOM.ts")
        self.appsink.set_property("emit-signals", True)
