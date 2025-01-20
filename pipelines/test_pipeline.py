import functools

from common.base_logger import logger
from elements import X264enc, H264ParseWrapper, RTPH264Pay, WebRTCBinWrapper
from elements.appsink import VideoAppSink, DataAppSink
from elements.fakesink import FakeSinkWrapper
from elements.videotestsrc import VideoTestSrcWrapper
from pipelines.base_pipeline import BaseSinkPipeline
from gi.repository import Gst


class TESTStreamPipeline(BaseSinkPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (VideoTestSrcWrapper(),
                                               X264enc(),
                                               H264ParseWrapper(),
                                               RTPH264Pay(),
                                               VideoAppSink(),
                                               DataAppSink(),
                                               WebRTCBinWrapper(),
                                               FakeSinkWrapper()
                                               )

        (self.videotestsrc, self.x264enc, self.h264parse, self.rtph264pay,
         self.videosink, self.datasink, self.webrtcbin, self.fakesink) = initialized_pipeline_elements_tuple

        elements = [self.videotestsrc, self.x264enc, self.h264parse, self.rtph264pay, self.videosink,
                    self.datasink, self.webrtcbin, self.fakesink]

        super().has_elements_initialized(elements)
        self.videosink.set_property("emit-signals", True)
        self.fakesink.set_property("dump", True)

    def on_data_sample(self, appsink) -> Gst.FlowReturn:
        try:
            sample = appsink.pull_sample()
            print(sample)
            # buffer = sample.get_buffer()
            # succ, info = buffer.map(Gst.MapFlags.READ)
            # buffer.unmap(info)
            # json_data = json.loads(info.data.decode('utf-8'))
            # self.data_queue.put(json_data)
        except Exception as e:
            logger.error("In data_sample: %s", e)
        return Gst.FlowReturn.OK

    def create_pipeline(self):
        try:
            self._instance.add(self.videotestsrc.get_element())
            self._instance.add(self.x264enc.get_element())
            self._instance.add(self.h264parse.get_element())
            self._instance.add(self.rtph264pay.get_element())
            self._instance.add(self.webrtcbin.get_element())
            self._instance.add(self.videosink.get_element())
            # self._instance.add(self.datasink.get_element())
            self._instance.add(self.fakesink.get_element())
        except Exception as e:
            logger.error("While adding elements to the pipeline: %s", e)

        self.videosink.connect("new-sample", self.on_data_sample)

        self.videotestsrc.link(self.x264enc)
        self.x264enc.link(self.h264parse)
        self.h264parse.link(self.rtph264pay)
        self.rtph264pay.link(self.videosink)

        return self._instance

    def start_pipeline(self, data) -> None:
        pass

    def stop_pipeline(self) -> None:
        pass
