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
    UDPSrcWrapper
)
from common.base_logger import logger
from elements.appsink import VideoAppSink
from elements.tsparse import TSParse

from pipelines.mp2t_pipeline import MP2TStreamPipeline


class UDPSRCPipeline(MP2TStreamPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (UDPSrcWrapper(),
                                               TSParse(),
                                               TSDemuxWrapper(),
                                               H264ParseWrapper(),
                                               RTPH264Pay(),
                                               VideoAppSink()
                                               )

        (self.udpsrc, self.tsparse, self.tsdemux, self.h264parse,
         self.rtph264pay, self.videosink) = initialized_pipeline_elements_tuple

        # elements = [self.udpsrc, self.tsdemux, self.h265parse, self.nvh265dec, self.x264enc, self.h264parse,
        #             self.rtph264pay, self.videosink]
        #
        #
        # super().has_elements_initialized(elements)

        self.tsparse.set_property("alignment", 7)
        AddressTuple = namedtuple("AddressTuple", ["ip", "port", "iface"])
        addr = AddressTuple(ip="127.0.0.1", port=5000, iface="lo")
        self.udpsrc.set_properties(addr)

        # self.filesrc.set_property("location", "/home/elbit/Desktop/flights/VNIR_ZOOM.ts")
        self.videosink.set_property("emit-signals", True)
        # self.x264enc.set_property("tune", "zerolatency")
        self.videosink.set_property("sync", False)

        # self.udpsrc.set_properties(ip="239.10.1.102", port=6022, iface="lo")

    def create_pipeline(self):
        try:
            self._instance.add(self.udpsrc.get_element())
            self._instance.add(self.tsparse.get_element())
            self._instance.add(self.tsdemux.get_element())
            # self._instance.add(self.h265parse.get_element())
            # self._instance.add(self.nvh265dec.get_element())
            # self._instance.add(self.x264enc.get_element())
            self._instance.add(self.h264parse.get_element())
            self._instance.add(self.rtph264pay.get_element())
            self._instance.add(self.videosink.get_element())
        except Exception as e:
            logger.error("While adding elements to the pipeline: %s", e)

        self.udpsrc.link(self.tsdemux)
        self.tsparse.link(self.tsdemux)
        self.tsdemux.connect("pad-added",
                             functools.partial(self.tsdemux.on_pad_added, elements=self.h264parse.get_element()))
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

        # self.h265parse.link(self.nvh265dec)
        # self.nvh265dec.link(self.x264enc)
        # self.x264enc.link(self.h264parse)
        self.h264parse.link(self.rtph264pay)
        self.rtph264pay.link(self.videosink)
        return self._instance
