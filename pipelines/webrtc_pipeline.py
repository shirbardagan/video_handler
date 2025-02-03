from common.base_logger import logger
from elements import WebRTCBinWrapper, CapsFilterWrapper, RTPH264DePayWrapper, H264ParseWrapper, NVH265DecWrapper, \
    VideoConvertWrapper, AutoVideoSinkWrapper
from elements.appsrc import VideoAppSrc, DataAppSrc
from elements.avdec_h264 import AVDecH264Wrapper
from elements.h264parse1 import H264Parse1Wrapper
from elements.identity import Identity
from elements.queue import QueueWrapper
from pipelines.base_pipeline import BaseSrcPipeline

import gi

gi.require_version("GstSdp", "1.0")
from gi.repository import Gst, GObject, GstSdp


class WebRTCPipeline(BaseSrcPipeline):
    def __init__(self):
        try:
            super().__init__()
            self._instance = Gst.Pipeline.new("pipeline")
            initialized_pipeline_elements_tuple = (VideoAppSrc(),
                                                   QueueWrapper(),
                                                   # RTPH264DePayWrapper(),
                                                   # H264Parse1Wrapper("h264parse"),
                                                   # AVDecH264Wrapper(),
                                                   # VideoConvertWrapper(),
                                                   # AutoVideoSinkWrapper()
                                                   # Identity(),
                                                   CapsFilterWrapper(),
                                                   # DataAppSrc(),
                                                   WebRTCBinWrapper()
                                                   )

            (self.videosrc, self.queue, self.capsfilter, self.webrtcbin) = initialized_pipeline_elements_tuple
            # (self.videosrc, self.rtph264depay, self.h264parse, self.avdec264, self.videoconvert, self.autovideosink) = initialized_pipeline_elements_tuple

            # elements = [self.videosrc, self.webrtcbin]
            #
            # if not all(elements):
            #     logger.error("Not all elements could be created.")

            video_caps = Gst.Caps.from_string(
                "application/x-rtp, media=(string)video, encoding-name=(string)H264, payload=(int)96, clock-rate=90000"
            )
            self.videosrc.connect("need-data", self.videosrc.on_need_data)
            # self.videosrc.connect("enough-data", self.videosrc.on_enough_data)
            self.videosrc.set_property("format", Gst.Format.TIME)
            self.videosrc.set_property("is-live", True)
            self.videosrc.set_property("do-timestamp", True)
            # self.videosrc.set_property("leaky-type", 2)
            # self.identity.set_property("dump", True)
            self.capsfilter.set_property("caps", video_caps)
            self.queue.set_property("max-size-buffers", 1)

        except Exception as e:
            logger.error("While initializing webrtcbin pipeline: %s", e)

    def create_pipeline(self):
        self._instance.add(self.videosrc.get_element())
        # self._instance.add(self.datasrc.get_element())
        # self._instance.add(self.rtph264depay.get_element())
        # self._instance.add(self.h264parse.get_element())
        # self._instance.add(self.avdec264.get_element())
        # self._instance.add(self.videoconvert.get_element())
        # self._instance.add(self.autovideosink.get_element())
        # self._instance.add(self.identity.get_element())
        self._instance.add(self.webrtcbin.get_element())
        self._instance.add(self.capsfilter.get_element())
        self._instance.add(self.queue.get_element())
        # self.videosrc.get_element().get_static_pad("src").add_probe(Gst.PadProbeType.BUFFER, self.on_probe)
        # self.webrtcbin.get_element().request_pad_simple("sink_%u").add_probe(Gst.PadProbeType.BUFFER,
        #                                                                      self.on_probe_webrtcbin)

        self.videosrc.link(self.queue)
        self.queue.link(self.capsfilter)
        self.capsfilter.link(self.webrtcbin)
        # self.videosrc.link(self.rtph264depay)
        # self.rtph264depay.link(self.h264parse)
        # self.h264parse.link(self.avdec264)
        # self.avdec264.link(self.videoconvert)
        # self.videoconvert.link(self.autovideosink)
        return self._instance

    def on_probe(self, pad, probe_info):
        print(probe_info.get_buffer().pts)
        return Gst.PadProbeReturn.OK

    def on_probe_webrtcbin(self, pad, probe_info):
        print("aaaaaaaaaaaaaaaaaaaaaaa")
        return Gst.PadProbeReturn.OK
