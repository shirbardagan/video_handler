import functools

from elements import (
    FileSrcWrapper,
    TSDemuxWrapper,
    H265ParseWrapper,
    NVH265DecWrapper,
    H264ParseWrapper,
    RTPH264Pay,
    CapsFilterWrapper,
    KLVParseWrapper,
    VideoAppSink,
    DataAppSink,
    NVH264EncWrapper
)
from pipelines.mp2t_pipeline import MP2TStreamPipeline

import gi
from gi.repository import Gst

gi.require_version('Gst', '1.0')


class MP2TH265StreamPipeline(MP2TStreamPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (FileSrcWrapper("filesrc"),
                                               TSDemuxWrapper("tsdemux"),
                                               KLVParseWrapper("klvparse"),
                                               DataAppSink("datasink"),
                                               H265ParseWrapper("h265parse"),
                                               NVH265DecWrapper("nvh265dec"),
                                               NVH264EncWrapper("nvh264enc"),
                                               CapsFilterWrapper("capsfilter"),
                                               H264ParseWrapper("h264parse"),
                                               RTPH264Pay("rtph264pay"),
                                               VideoAppSink("videosink")
                                               )

        (self.filesrc, self.tsdemux, self.klvparse, self.datasink, self.h265parse, self.nvh265dec, self.nvh264enc,
         self.capsfilter, self.h264parse, self.rtph264pay, self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.filesrc, self.tsdemux, self.h265parse, self.nvh265dec, self.nvh264enc, self.h264parse,
                    self.rtph264pay, self.videosink]

        super().has_elements_initialized(elements)

        self.filesrc.set_property("location", "/home/elbit/Desktop/flights/VNIR_ZOOM.ts")

        self.videosink.set_property("emit-signals", True)
        self.datasink.set_property("emit-signals", True)

        self.capsfilter.set_property("caps",
            "video/x-h264, stream-format=(string)byte-stream, alignment=(string)au, level=(string)4, profile=(string)main")

    def create_pipeline(self) -> Gst.Pipeline:
        self._add_elements()
        self._connect_signals()
        self._link_elements()
        return self._instance

    def _add_elements(self):
        elements_to_add = [
            self.filesrc, self.tsdemux, self.klvparse, self.datasink, self.h265parse, self.nvh265dec,
            self.nvh264enc, self.capsfilter, self.h264parse, self.rtph264pay, self.videosink
        ]
        self.add_elements(elements_to_add)

    def _connect_signals(self):
        self.tsdemux.connect("pad-added",
                             functools.partial(self.tsdemux.on_pad_added,
                                               elements=[self.h265parse.get_element(), self.klvparse.get_element()]))
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))
        self.datasink.connect("new-sample", functools.partial(self.datasink.on_data_sample))

    def _link_elements(self):
        links = [
            (self.filesrc, self.tsdemux),
            (self.klvparse, self.datasink),
            (self.h265parse, self.nvh265dec),
            (self.nvh265dec, self.nvh264enc),
            (self.nvh264enc, self.capsfilter),
            (self.capsfilter, self.h264parse),
            (self.h264parse, self.rtph264pay),
            (self.rtph264pay, self.videosink),
        ]
        super().link_elements(links)