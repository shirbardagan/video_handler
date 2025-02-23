import functools
from collections import namedtuple
from elements import (
    TSDemuxWrapper,
    H265ParseWrapper,
    NVH265DecWrapper,
    H264ParseWrapper,
    RTPH264Pay,
    UDPSrcWrapper, CapsFilterWrapper, KLVParseWrapper
)
from common.base_logger import logger
from elements.appsink import VideoAppSink, DataAppSink
from elements.nvh264enc import NVH264EncWrapper

from pipelines.mp2t_pipeline import MP2TStreamPipeline

import gi
from gi.repository import Gst

gi.require_version('Gst', '1.0')


class UDPSRCPipeline(MP2TStreamPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (UDPSrcWrapper(),
                                               TSDemuxWrapper(),
                                               KLVParseWrapper(),
                                               DataAppSink("datasink"),
                                               H265ParseWrapper(),
                                               NVH265DecWrapper(),
                                               NVH264EncWrapper(),
                                               CapsFilterWrapper("capsfilter1"),
                                               H264ParseWrapper(),
                                               RTPH264Pay(),
                                               VideoAppSink()
                                               )

        (self.udpsrc, self.tsdemux, self.klvparse, self.datasink, self.h265parse, self.nvh265dec, self.nvh264enc,
         self.capsfilter, self.h264parse,
         self.rtph264pay, self.videosink) = initialized_pipeline_elements_tuple

        # elements = [self.udpsrc, self.tsdemux, self.h265parse, self.nvh265dec, self.x264enc, self.h264parse,
        #             self.rtph264pay, self.videosink]
        #
        #
        # super().has_elements_initialized(elements)

        AddressTuple = namedtuple("AddressTuple", ["ip", "port", "iface"])
        addr = AddressTuple(ip="239.3.43.3", port=6146, iface="lo")
        self.udpsrc.set_properties(addr)

        self.videosink.set_property("emit-signals", True)
        self.videosink.set_property("sync", False)
        self.datasink.set_property("emit-signals", True)

        self.rtph264pay.set_property("config-interval", -1)

    def create_pipeline(self) -> Gst.Pipeline:
        self._add_elements()
        self._connect_signals()
        self._link_elements()
        return self._instance

    def _add_elements(self):
        elements_to_add = [
            self.udpsrc, self.tsdemux, self.klvparse, self.datasink, self.h265parse, self.nvh265dec,
            self.nvh264enc, self.capsfilter, self.h264parse, self.rtph264pay, self.videosink
        ]
        self.add_elements(elements_to_add)

    def _connect_signals(self):
        self.tsdemux.connect("pad-added",
                             functools.partial(self.tsdemux.on_pad_added,
                                               elements=[self.h265parse.get_element(),
                                                         self.klvparse.get_element()]))
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))
        self.datasink.connect("new-sample", functools.partial(self.datasink.on_data_sample))

    def _link_elements(self):
        links = [
            (self.udpsrc, self.tsdemux),
            (self.klvparse, self.datasink),
            (self.h265parse, self.nvh265dec),
            (self.nvh265dec, self.nvh264enc),
            (self.nvh264enc, self.capsfilter),
            (self.capsfilter, self.h264parse),
            (self.h264parse, self.rtph264pay),
            (self.rtph264pay, self.videosink),
        ]
        super().link_elements(links)


