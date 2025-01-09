import functools

from gi.overrides import override

from pipelines.base_pipeline import BaseStreamPipeline

from elements import (
    FileSrcWrapper,
    TSDemuxWrapper,
    H265ParseWrapper,
    NVH265DecWrapper,
    H264ParseWrapper,
    RTPH264Pay,
    X264enc, AppSinkWrapper
)
from common.base_logger import logger
from gi.repository import Gst, GObject

from pipelines.mp2t_pipeline import MP2TStreamPipeline


class MP2TH265StreamPipeline(MP2TStreamPipeline):
    def __init__(self):
        self._instance = Gst.Pipeline.new("pipeline")
        initialized_pipeline_elements_tuple = (FileSrcWrapper("filesrc"),
                                               TSDemuxWrapper("tsdemux"),
                                               H265ParseWrapper("h265parse"),
                                               NVH265DecWrapper("nvh265dec"),
                                               X264enc("x264enc"),
                                               H264ParseWrapper("h264parse"),
                                               RTPH264Pay("rtph264pay"),
                                               AppSinkWrapper("appsink")
                                               )

        (self.filesrc, self.tsdemux, self.h265parse, self.nvh265dec, self.x264enc, self.h264parse,
         self.rtph264pay, self.appsink) = initialized_pipeline_elements_tuple

        elements = [self.filesrc, self.tsdemux, self.h265parse, self.nvh265dec, self.x264enc, self.h264parse,
                    self.rtph264pay, self.appsink]

        if not all(elements):
            logger.error("Not all elements could be created.")

        self.filesrc.set_property("location", "/home/elbit/Desktop/flights/VNIR_ZOOM.ts")
        self.appsink.set_property("emit-signals", True)

    @override
    def create_pipeline(self):
        try:
            self._instance.add(self.filesrc.get_element())
            self._instance.add(self.tsdemux.get_element())
            self._instance.add(self.h265parse.get_element())
            self._instance.add(self.nvh265dec.get_element())
            self._instance.add(self.x264enc.get_element())
            self._instance.add(self.h264parse.get_element())
            self._instance.add(self.rtph264pay.get_element())
            self._instance.add(self.webrtcbin.get_element())
        except Exception as e:
            logger.error("While adding elements to the pipeline: %s", e)

        self.filesrc.link(self.tsdemux)
        self.tsdemux.get_element().connect("pad-added",
                                      functools.partial(self.tsdemux.on_pad_added, elements=self.h265parse.get_element()))
        self.appsink.connect("new-sample", functools.partial(self.on_data_sample))

        self.h265parse.link(self.nvh265dec)
        self.nvh265dec.link(self.x264enc)
        self.x264enc.link(self.h264parse)
        self.h264parse.link(self.rtph264pay)
        rtph264pay_src_pad = self.rtph264pay.get_element().get_static_pad("src")
        return self._instance