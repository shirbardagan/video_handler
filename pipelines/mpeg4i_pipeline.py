import functools

from common.base_logger import logger
from elements import FileSrcWrapper, H264ParseWrapper, RTPH264Pay, VideoAppSink
from elements.qtdemux import QTDemuxWrapper
from pipelines import BaseStreamPipeline


class MPEG4IStreamPipeline(BaseStreamPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (FileSrcWrapper(),
                                               QTDemuxWrapper(),
                                               H264ParseWrapper(),
                                               RTPH264Pay(),
                                               VideoAppSink()
                                               )

        (self.filesrc, self.qtdemux, self.h264parse, self.rtph264pay,
         self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.filesrc, self.qtdemux, self.h264parse, self.rtph264pay, self.videosink]
        self.has_elements_initialized(elements)

        self.filesrc.set_property("location", "/home/elbit/Downloads/santa.mp4")
        self.h264parse.set_property("config-interval", -1)
        self.rtph264pay.set_property("config-interval", -1)
        self.videosink.set_property("emit-signals", True)
        # self.videosink.set_property("async", True)

    def create_pipeline(self):
        try:
            self._instance.add(self.filesrc.get_element())
            self._instance.add(self.qtdemux.get_element())
            self._instance.add(self.h264parse.get_element())
            self._instance.add(self.rtph264pay.get_element())
            self._instance.add(self.videosink.get_element())
        except Exception as e:
            logger.error("While adding elements to the pipeline: %s", e)

        self.filesrc.link(self.qtdemux)
        self.qtdemux.get_element().connect("pad-added",
                                           functools.partial(self.qtdemux.on_pad_added,
                                                             elements=self.h264parse.get_element()))
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))
        self.h264parse.link(self.rtph264pay)
        self.rtph264pay.link(self.videosink)

        return self._instance
