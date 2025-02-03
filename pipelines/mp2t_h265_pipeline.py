import functools

from elements import (
    FileSrcWrapper,
    TSDemuxWrapper,
    H265ParseWrapper,
    NVH265DecWrapper,
    H264ParseWrapper,
    RTPH264Pay,
    X264enc,
    WebRTCBinWrapper
)
from common.base_logger import logger
from elements.appsink import VideoAppSink
from elements.nvh264enc import NVH264EncWrapper

from pipelines.mp2t_pipeline import MP2TStreamPipeline


class MP2TH265StreamPipeline(MP2TStreamPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (FileSrcWrapper(),
                                               TSDemuxWrapper(),
                                               H265ParseWrapper(),
                                               NVH265DecWrapper(),
                                               NVH264EncWrapper(),
                                               H264ParseWrapper(),
                                               RTPH264Pay(),
                                               VideoAppSink()
                                               )

        (self.filesrc, self.tsdemux, self.h265parse, self.nvh265dec, self.nvh264enc, self.h264parse,
         self.rtph264pay, self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.filesrc, self.tsdemux, self.h265parse, self.nvh265dec, self.nvh264enc, self.h264parse,
                    self.rtph264pay, self.videosink]

        super().has_elements_initialized(elements)

        # self.filesrc.set_property("location", "/home/elbit/Desktop/flights/VNIR_ZOOM.ts")
        self.videosink.set_property("emit-signals", True)
        # self.nvh264enc.set_property("tune", "zerolatency")
        self.videosink.set_property("sync", False)
        self.h264parse.set_property("config-interval", -1)
        self.nvh264enc.set_property("gop-size", 8)
        self.nvh264enc.set_property("aud", True)
        # self.nvh264enc.set_property("present", "low-latency")
        self.h264parse.set_property("config-interval", -1)
        self.rtph264pay.set_property("config-interval", -1)

    def create_pipeline(self):
        try:
            self._instance.add(self.filesrc.get_element())
            self._instance.add(self.tsdemux.get_element())
            self._instance.add(self.h265parse.get_element())
            self._instance.add(self.nvh265dec.get_element())
            self._instance.add(self.nvh264enc.get_element())
            self._instance.add(self.h264parse.get_element())
            self._instance.add(self.rtph264pay.get_element())
            self._instance.add(self.videosink.get_element())
        except Exception as e:
            logger.error("While adding elements to the pipeline: %s", e)

        self.filesrc.link(self.tsdemux)
        self.tsdemux.connect("pad-added",
                             functools.partial(self.tsdemux.on_pad_added, elements=self.h265parse.get_element()))
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

        self.h265parse.link(self.nvh265dec)
        self.nvh265dec.link(self.nvh264enc)
        self.nvh264enc.link(self.h264parse)
        self.h264parse.link(self.rtph264pay)
        self.rtph264pay.link(self.videosink)

        return self._instance
