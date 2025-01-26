import functools

from common.base_logger import logger
from elements import FileSrcWrapper, H264ParseWrapper, RTPH264Pay, WebRTCBinWrapper
from elements.qtdemux import QTDemuxWrapper
from pipelines.base_pipeline import BaseSinkPipeline


class MPEG4IStreamPipeline(BaseSinkPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (FileSrcWrapper(),
                                               QTDemuxWrapper(),
                                               H264ParseWrapper(),
                                               RTPH264Pay(),
                                               WebRTCBinWrapper()
                                               )

        (self.filesrc, self.qtdemux, self.h264parse, self.rtph264pay,
         self.webrtcbin) = initialized_pipeline_elements_tuple

        elements = [self.filesrc, self.qtdemux, self.h264parse, self.rtph264pay, self.webrtcbin]
        super().has_elements_initialized(elements)

        # self.filesrc.set_property("location", "/home/elbit/Downloads/butt.mp4")
        self.h264parse.set_property("config-interval", -1)
        self.rtph264pay.set_property("config-interval", -1)
        # self.appsink.set_property("emit-signals", True)

    def create_pipeline(self):
        try:
            self._instance.add(self.filesrc.get_element())
            self._instance.add(self.qtdemux.get_element())
            self._instance.add(self.h264parse.get_element())
            self._instance.add(self.rtph264pay.get_element())
            self._instance.add(self.webrtcbin.get_element())
        except Exception as e:
            logger.error("While adding elements to the pipeline: %s", e)

        self.filesrc.link(self.qtdemux)
        self.qtdemux.get_element().connect("pad-added",
                                           functools.partial(self.qtdemux.on_pad_added,
                                                             elements=self.h264parse.get_element()))
        # self.appsink.connect("new-sample", functools.partial(self.on_data_sample))
        self.h264parse.link(self.rtph264pay)
        self.rtph264pay.link(self.webrtcbin)

        return self._instance

    def start_pipeline(self, data) -> None:
        pass

    def stop_pipeline(self) -> None:
        pass
