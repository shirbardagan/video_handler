import functools

from elements import (
    FileSrcWrapper,
    TSDemuxWrapper,
    H265ParseWrapper,
    NVH265DecWrapper,
    H264ParseWrapper,
    RTPH264Pay,
    X264enc,
    WebRTCBinWrapper, CapsFilterWrapper
)
from common.base_logger import logger
from elements.appsink import VideoAppSink
from elements.nvh264enc import NVH264EncWrapper
from elements.queue import QueueWrapper
from elements.queue1 import Queue1Wrapper

from pipelines.mp2t_pipeline import MP2TStreamPipeline

import gi
from gi.repository import Gst

gi.require_version('Gst', '1.0')


class AAA(MP2TStreamPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (FileSrcWrapper(),
                                               TSDemuxWrapper(),
                                               H265ParseWrapper(),
                                               NVH265DecWrapper(),
                                               Queue1Wrapper(),
                                               NVH264EncWrapper(),
                                               # CapsFilterWrapper(),
                                               H264ParseWrapper(),
                                               RTPH264Pay(),
                                               CapsFilterWrapper(),
                                               VideoAppSink()
                                               )

        (self.filesrc, self.tsdemux, self.h265parse, self.nvh265dec, self.queue, self.nvh264enc,
         self.h264parse,
         self.rtph264pay,self.capsfilter2, self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.filesrc, self.tsdemux, self.h265parse, self.nvh265dec, self.nvh264enc, self.h264parse,
                    self.rtph264pay, self.videosink]

        super().has_elements_initialized(elements)

        # self.filesrc.set_property("location", "/home/elbit/Desktop/flights/VNIR_ZOOM.ts")
        self.videosink.set_property("emit-signals", True)
        self.videosink.set_property("sync", False)
        self.videosink.set_property("async", False)
        self.h264parse.set_property("config-interval", -1)
        self.h265parse.set_property("config-interval", -1)
        self.nvh264enc.set_property("gop-size", 30)
        # self.nvh264enc.set_property("aud", True)
        self.nvh265dec.set_property("max-errors", -1)
        self.nvh264enc.set_property("preset", "low-latency")
        self.nvh264enc.set_property("bitrate", 4000)
        self.nvh264enc.set_property("zerolatency", False)
        self.rtph264pay.set_property("config-interval", -1)
        # self.capsfilter.set_property("caps",
        #                              Gst.Caps.from_string("video/x-h264,profile=constrained-baseline,alignment=au"))
        # self.rtph264pay.set_property("aggregate-mode","zero-latency")
        self.capsfilter2.set_property("caps", Gst.Caps.from_string(
            "application/x-rtp,media=(string)video, encoding-name=(string)H264, payload=(int)96,profile=main,alignment=au"))
        self.queue.set_property("max-size-buffers", 1)
        self.queue.set_property("max-size-time", 0)
        self.queue.set_property("max-size-bytes", 0)
        self.queue.set_property("leaky", "downstream")
        self.queue.set_property("silent", True)


    def create_pipeline(self):
        try:
            self._instance.add(self.filesrc.get_element())
            self._instance.add(self.tsdemux.get_element())
            self._instance.add(self.h265parse.get_element())
            self._instance.add(self.nvh265dec.get_element())
            self._instance.add(self.queue.get_element())
            self._instance.add(self.nvh264enc.get_element())
            # self._instance.add(self.capsfilter.get_element())
            self._instance.add(self.capsfilter2.get_element())
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
        self.nvh265dec.link(self.queue)
        self.queue.link(self.nvh264enc)
        self.nvh264enc.link(self.h264parse)
        # self.capsfilter.link(self.h264parse)
        self.h264parse.link(self.rtph264pay)
        self.rtph264pay.link(self.capsfilter2)
        self.capsfilter2.link(self.videosink)

        return self._instance
