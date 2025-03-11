import functools

from elements import (
    H265ParseWrapper,
    NVH265DecWrapper,
    H264ParseWrapper,
    RTPH264Pay,
    CapsFilterWrapper,
    VideoAppSink,
    NVH264EncWrapper
)
from elements.gldownload import GLDownloadWrapper
from pipelines.mp2t_pipeline import MP2TStreamPipeline

import gi
from gi.repository import Gst

gi.require_version('Gst', '1.0')


class MP2TH265StreamPipeline(MP2TStreamPipeline):
    def __init__(self):
        super().__init__()
        self._elements = []
        initialized_pipeline_elements_tuple = (H265ParseWrapper("h265parser"),
                                               NVH265DecWrapper("h265decoder"),
                                               GLDownloadWrapper("gldownload"),
                                               NVH264EncWrapper("h264encoder"),
                                               CapsFilterWrapper("h264capsfilter"),
                                               H264ParseWrapper("h264parser"),
                                               RTPH264Pay("rtph264pay"),
                                               VideoAppSink("videosink")
                                               )

        (self.h265parse, self.nvh265dec, self.gldownload, self.nvh264enc,
         self.capsfilter, self.h264parse, self.rtph264pay, self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.h265parse, self.nvh265dec, self.gldownload, self.nvh264enc, self.h264parse, self.rtph264pay, self.videosink]

        self.has_elements_initialized(elements)
        self.nvh265dec.set_property("max-errors", -1)
        self.nvh264enc.set_property("bitrate", 4096)
        self.nvh264enc.set_property("preset", "low-latency-hp")
        self.nvh264enc.set_property("zerolatency", True)
        self.nvh264enc.set_property("gop-size", 30)

        self.h265parse.set_property("config-interval", -1)
        self.h264parse.set_property("config-interval", -1)
        self.rtph264pay.set_property("config-interval", -1)

        self.videosink.set_property("emit-signals", True)
        self.videosink.set_property("sync", False)
        self.videosink.set_property("async", False)

        self.capsfilter.set_property("caps",
            "video/x-h264, stream-format=(string)byte-stream, alignment=(string)au, level=(string)4, profile=(string)main")

    def create_pipeline(self) -> Gst.Pipeline:
        self._add_elements()
        self._connect_signals()
        self._link_elements()
        return self._instance

    def _add_elements(self):
        elements_to_add = [
            self.h265parse, self.nvh265dec, self.gldownload, self.nvh264enc, self.capsfilter, self.h264parse, self.rtph264pay, self.videosink
        ]
        self.add_elements(elements_to_add)

    def _connect_signals(self):
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

    def _link_elements(self):
        links = [
            (self.h265parse, self.nvh265dec),
            (self.nvh265dec, self.gldownload),
            (self.gldownload, self.nvh264enc),
            (self.nvh264enc, self.capsfilter),
            (self.capsfilter, self.h264parse),
            (self.h264parse, self.rtph264pay),
            (self.rtph264pay, self.videosink),
        ]
        self.link_elements(links)

    def get_parser(self):
        return self.h265parse