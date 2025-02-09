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


class MP2TH265StreamPipeline(MP2TStreamPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (FileSrcWrapper(),
                                               TSDemuxWrapper(),
                                               H265ParseWrapper(),
                                               CapsFilterWrapper("capsfilter0"),
                                               NVH265DecWrapper(),
                                               CapsFilterWrapper("capsfilter4"),
                                               NVH264EncWrapper(),
                                               CapsFilterWrapper("capsfilter1"),
                                               H264ParseWrapper(),
                                               CapsFilterWrapper("capsfilter3"),
                                               RTPH264Pay(),
                                               CapsFilterWrapper("capsfilter2"),
                                               VideoAppSink()
                                               )

        (self.filesrc, self.tsdemux, self.h265parse, self.capsfilter, self.nvh265dec, self.capsfilter4, self.nvh264enc, self.capsfilter1,
         self.h264parse, self.capsfilter3,
         self.rtph264pay, self.capsfilter2, self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.filesrc, self.tsdemux, self.h265parse, self.nvh265dec, self.nvh264enc, self.h264parse,
                    self.rtph264pay, self.videosink]

        super().has_elements_initialized(elements)

        self.filesrc.set_property("location", "/home/elbit/Desktop/flights/VNIR_ZOOM.ts")

        # self.videosink.set_property("emit-signals", True)

        # self.rtph264pay.set_property("config-interval", 1)

        self.capsfilter2.set_property("caps",Gst.Caps.from_string("application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, packetization-mode=(string)1, profile=(string)main, payload=(int)96"))
        self.capsfilter.set_property("caps", Gst.Caps.from_string("video/x-h265, stream-format=(string)hev1, width=(int)1920, height=(int)1080, chroma-format=(string)4:2:0, bit-depth-luma=(uint)8, bit-depth-chroma=(uint)8, parsed=(boolean)true, alignment=(string)au, profile=(string)main, tier=(string)main, level=(string)4"))
        self.capsfilter1.set_property("caps", Gst.Caps.from_string("video/x-h264, stream-format=(string)byte-stream, alignment=(string)au, level=(string)4, profile=(string)main, width=(int)1920, height=(int)1080, pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)0/1, interlace-mode=(string)progressive, colorimetry=(string)bt709, chroma-site=(string)mpeg2, multiview-mode=(string)mono, multiview-flags=(GstVideoMultiviewFlagsSet)0:ffffffff:/right-view-first/left-flipped/left-flopped/right-flipped/right-flopped/half-aspect/mixed-mono"))
        self.capsfilter3.set_property("caps", Gst.Caps.from_string("video/x-h264, stream-format=(string)avc, alignment=(string)au, level=(string)4, profile=(string)main, width=(int)1920, height=(int)1080, pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)0/1, interlace-mode=(string)progressive, colorimetry=(string)bt709, chroma-site=(string)mpeg2, multiview-mode=(string)mono, multiview-flags=(GstVideoMultiviewFlagsSet)0:ffffffff:/right-view-first/left-flipped/left-flopped/right-flipped/right-flopped/half-aspect/mixed-mono, coded-picture-structure=(string)frame, chroma-format=(string)4:2:0, bit-depth-luma=(uint)8, bit-depth-chroma=(uint)8, parsed=(boolean)true"))
        self.capsfilter4.set_property("caps", Gst.Caps.from_string("video/x-raw(memory:CUDAMemory), format=(string)NV12, width=(int)1920, height=(int)1080, interlace-mode=(string)progressive, multiview-mode=(string)mono, pixel-aspect-ratio=(fraction)1/1, framerate=(fraction)0/1"))

    def create_pipeline(self):
        try:
            self._instance.add(self.filesrc.get_element())
            self._instance.add(self.tsdemux.get_element())
            self._instance.add(self.h265parse.get_element())
            self._instance.add(self.capsfilter.get_element())
            self._instance.add(self.capsfilter1.get_element())
            self._instance.add(self.capsfilter2.get_element())
            self._instance.add(self.capsfilter3.get_element())
            self._instance.add(self.capsfilter4.get_element())
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
        # self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

        self.h265parse.link(self.capsfilter)
        self.capsfilter.link(self.nvh265dec)
        self.nvh265dec.link(self.capsfilter4)
        self.capsfilter4.link(self.nvh264enc)
        self.nvh264enc.link(self.capsfilter1)
        self.capsfilter1.link(self.h264parse)
        self.h264parse.link(self.rtph264pay)
        # self.capsfilter3.link(self.rtph264pay)
        self.rtph264pay.link(self.capsfilter2)
        self.capsfilter2.link(self.videosink)

        return self._instance


    def create_mpeg_pipeline(self):
        # udpsrc address=239.253.4.2 port=6000 multicast-iface=lo
        pipeline_str = f""" filesrc location=/home/elbit/Desktop/flights/VNIR_ZOOM.ts
         ! tsdemux ! h265parse name=h265parse ! nvh265dec name=nvh265dec !
         nvh264enc name=nvh264enc ! h264parse name=h264parse ! rtph264pay name=rtph264pay config-interval=-1
           ! appsink name=videosink """

        return pipeline_str
