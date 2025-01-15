from common.base_logger import logger
from elements import X264enc, H264ParseWrapper, RTPH264Pay, WebRTCBinWrapper
from elements.videotestsrc import VideoTestSrcWrapper
from pipelines.base_pipeline import BaseSinkPipeline


class TESTStreamPipeline(BaseSinkPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (VideoTestSrcWrapper(),
                                               X264enc(),
                                               H264ParseWrapper(),
                                               RTPH264Pay(),
                                               WebRTCBinWrapper()
                                               )

        (self.videotestsrc, self.x264enc, self.h264parse, self.rtph264pay,
         self.webrtcbin) = initialized_pipeline_elements_tuple

        elements = [self.videotestsrc, self.x264enc, self.h264parse, self.rtph264pay, self.webrtcbin]

        super().has_elements_initialized(elements)

    def create_pipeline(self):
        try:
            self._instance.add(self.videotestsrc.get_element())
            self._instance.add(self.x264enc.get_element())
            self._instance.add(self.h264parse.get_element())
            self._instance.add(self.rtph264pay.get_element())
            self._instance.add(self.webrtcbin.get_element())
        except Exception as e:
            logger.error("While adding elements to the pipeline: %s", e)

        # self.appsink.connect("new-sample", functools.partial(self.on_data_sample))

        self.videotestsrc.link(self.x264enc)
        self.x264enc.link(self.h264parse)
        self.h264parse.link(self.rtph264pay)
        self.rtph264pay.link(self.webrtcbin)

        return self._instance

    def start_pipeline(self, data) -> None:
        pass

    def stop_pipeline(self) -> None:
        pass
