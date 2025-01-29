import functools

from elements import (
    FileSrcWrapper,
    TSDemuxWrapper,
    H265ParseWrapper,
    NVH265DecWrapper,
    H264ParseWrapper,
    RTPH264Pay,
    X264enc,
    VideoAppSink, WebRTCBinWrapper, VideoAppSrc, Identity, LibDe265DecWrapper)
from common.base_logger import logger
from elements.capsfilter import CapsFilterWrapper

from pipelines.mp2t_pipeline import MP2TStreamPipeline
from pipelines.webrtc_pipeline import WebRTCPipeline

import gi

gi.require_version('Gst', '1.0')
from gi.repository import Gst

class MP2TH265StreamPipeline(MP2TStreamPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (FileSrcWrapper(),
                                               TSDemuxWrapper(),
                                               H265ParseWrapper(),
                                               LibDe265DecWrapper(),
                                               X264enc(),
                                               H264ParseWrapper(),
                                               RTPH264Pay(),
                                               VideoAppSink(),
                                               VideoAppSrc(),
                                               Identity(),
                                               CapsFilterWrapper(),
                                               WebRTCBinWrapper()
                                               )

        (self.filesrc, self.tsdemux, self.h265parse, self.libde265dec, self.x264enc, self.h264parse,
         self.rtph264pay, self.videosink, self.videosrc, self.identity, self.capsfilter, self.webrtcbin) = initialized_pipeline_elements_tuple

        elements = [self.filesrc, self.tsdemux, self.h265parse, self.libde265dec, self.x264enc, self.h264parse,
                    self.rtph264pay, self.videosink]

        super().has_elements_initialized(elements)
        video_caps = Gst.Caps.from_string(
            "application/x-rtp, media=(string)video, encoding-name=(string)H264, payload=(int)96, clock-rate=(int)90000"
        )
        self.filesrc.set_property("location", "/home/shir/Desktop/flights/VNIR_ZOOM.ts")
        self.videosink.set_property("emit-signals", True)
        # self.x264enc.set_property("tune", "zerolatency")
        self.videosink.set_property("sync", 0)
        self.capsfilter.set_property("caps", video_caps)

        if not all(elements):
            logger.error("Not all elements could be created.")
        self.videosrc.connect("need-data", self.videosrc.on_need_data)
        self.videosrc.connect("enough-data", self.videosrc.on_enough_data)
        self.videosrc.set_property("format", Gst.Format.TIME)
        self.videosrc.set_property("is-live", True)
        self.videosrc.set_property("do-timestamp", True)
        # self.videosrc.set_property("leaky-type", 2)
        self.identity.set_property("dump", True)

    def create_pipeline(self):
        try:
            self._instance.add(self.filesrc.get_element())
            self._instance.add(self.tsdemux.get_element())
            self._instance.add(self.h265parse.get_element())
            self._instance.add(self.libde265dec.get_element())
            self._instance.add(self.x264enc.get_element())
            self._instance.add(self.h264parse.get_element())
            self._instance.add(self.rtph264pay.get_element())
            self._instance.add(self.videosink.get_element())
            self._instance.add(self.videosrc.get_element())
            self._instance.add(self.identity.get_element())
            self._instance.add(self.capsfilter.get_element())
            self._instance.add(self.webrtcbin.get_element())
        except Exception as e:
            logger.error("While adding elements to the pipeline: %s", e)

        self.filesrc.link(self.tsdemux)
        self.tsdemux.connect("pad-added",
                             functools.partial(self.tsdemux.on_pad_added, elements=self.h265parse.get_element()))
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

        self.h265parse.link(self.libde265dec)
        self.libde265dec.link(self.x264enc)
        self.x264enc.link(self.h264parse)
        self.h264parse.link(self.rtph264pay)
        self.rtph264pay.link(self.videosink)

        self.videosrc.link(self.identity)
        self.identity.link(self.capsfilter)
        self.capsfilter.link(self.webrtcbin)

        return self._instance
