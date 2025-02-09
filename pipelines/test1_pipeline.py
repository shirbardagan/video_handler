import functools

from app_instance import app
from common.base_logger import logger
from elements import X264enc, H264ParseWrapper, RTPH264Pay, WebRTCBinWrapper, X265enc, H265ParseWrapper, \
    NVH265DecWrapper
from elements.appsink import VideoAppSink, DataAppSink
from elements.fakesink import FakeSinkWrapper
from elements.videotestsrc import VideoTestSrcWrapper
from pipelines.base_pipeline import BaseSinkPipeline
from gi.repository import Gst


class TEST1StreamPipeline(BaseSinkPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (VideoTestSrcWrapper(),
                                               X265enc(),
                                               H265ParseWrapper(),
                                               NVH265DecWrapper(),
                                               X264enc(),
                                               H264ParseWrapper(),
                                               RTPH264Pay(),
                                               VideoAppSink(),
                                               # DataAppSink(),
                                               # WebRTCBinWrapper(),
                                               # FakeSinkWrapper()
                                               )

        (self.videotestsrc, self.x265enc, self.h265parse, self.nv265dec, self.x264enc, self.h264parse, self.rtph264pay,
         self.videosink) = initialized_pipeline_elements_tuple

        # elements = [self.videotestsrc, self.x264enc, self.h264parse, self.rtph264pay, self.videosink,
        #             self.datasink, self.webrtcbin, self.fakesink]
        #
        # super().has_elements_initialized(elements)
        self.videosink.set_property("emit-signals", True)
        # self.fakesink.set_property("dump", True)
        self.videotestsrc.set_property("pattern", 18)
        self.videosink.set_property("sync", False)

    def create_pipeline(self):
        try:
            self._instance.add(self.videotestsrc.get_element())
            # self._instance.add(self.x265enc.get_element())
            self._instance.add(self.h265parse.get_element())
            self._instance.add(self.nv265dec.get_element())
            self._instance.add(self.x264enc.get_element())
            self._instance.add(self.h264parse.get_element())
            self._instance.add(self.rtph264pay.get_element())
            # self._instance.add(self.webrtcbin.get_element())
            self._instance.add(self.videosink.get_element())
            # self._instance.add(self.datasink.get_element())
            # self._instance.add(self.fakesink.get_element())
        except Exception as e:
            logger.error("While adding elements to the pipeline: %s", e)

        self.videosink.connect("new-sample", self.videosink.on_data_sample)

        self.videotestsrc.link(self.x265enc)
        self.x265enc.link(self.h265parse)
        self.h265parse.link(self.nv265dec)
        self.nv265dec.link(self.x264enc)
        self.x264enc.link(self.h264parse)
        self.h264parse.link(self.rtph264pay)
        self.rtph264pay.link(self.videosink)

        return self._instance

    def start_pipeline(self) -> None:
        pass

    def stop_pipeline(self) -> None:
        pass
