import functools
from collections import namedtuple
from elements import (
    FileSrcWrapper,
    TSDemuxWrapper,
    H265ParseWrapper,
    NVH265DecWrapper,
    H264ParseWrapper,
    RTPH264Pay,
    X264enc,
    WebRTCBinWrapper,
    UDPSrcWrapper, CapsFilterWrapper
)
from common.base_logger import logger
from elements.appsink import VideoAppSink
from elements.nvh264enc import NVH264EncWrapper
from elements.tsparse import TSParse

from pipelines.mp2t_pipeline import MP2TStreamPipeline


class UDPSRCPipeline(MP2TStreamPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (UDPSrcWrapper(),
                                               TSDemuxWrapper(),
                                               H265ParseWrapper(),
                                               NVH265DecWrapper(),
                                               NVH264EncWrapper(),
                                               CapsFilterWrapper("capsfilter1"),
                                               H264ParseWrapper(),
                                               RTPH264Pay(),
                                               VideoAppSink()
                                               )

        (self.udpsrc, self.tsdemux, self.h265parse, self.nvh265dec, self.nvh264enc, self.capsfilter, self.h264parse,
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
        self.rtph264pay.set_property("config-interval", -1)

    def create_pipeline(self):
        try:
            self._instance.add(self.udpsrc.get_element())
            self._instance.add(self.tsdemux.get_element())
            self._instance.add(self.h265parse.get_element())
            self._instance.add(self.capsfilter.get_element())
            self._instance.add(self.nvh265dec.get_element())
            self._instance.add(self.nvh264enc.get_element())
            self._instance.add(self.h264parse.get_element())
            self._instance.add(self.rtph264pay.get_element())
            self._instance.add(self.videosink.get_element())
        except Exception as e:
            logger.error("While adding elements to the pipeline: %s", e)

        self.udpsrc.link(self.tsdemux)
        self.tsdemux.connect("pad-added",
                             functools.partial(self.tsdemux.on_pad_added, elements=self.h265parse.get_element()))
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

        self.h265parse.link(self.nvh265dec)
        self.nvh265dec.link(self.nvh264enc)
        self.nvh264enc.link(self.capsfilter)
        self.capsfilter.link(self.h264parse)
        self.h264parse.link(self.rtph264pay)
        self.rtph264pay.link(self.videosink)
        return self._instance
