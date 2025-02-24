import functools

from elements import TSDemuxWrapper, FileSrcWrapper
from pipelines.base_pipeline import BaseStreamPipeline
from common.base_logger import logger


class MP2TStreamPipeline(BaseStreamPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (FileSrcWrapper(),
                                               TSDemuxWrapper())
        (self.filesrc, self.tsdemux) = initialized_pipeline_elements_tuple

        elements = [self.filesrc, self.tsdemux]
        super().has_elements_initialized(elements)

        # self.filesrc.set_property("location", "/home/elbit/Desktop/flights/VNIR_ZOOM.ts")

    def create_pipeline(self):
        try:
            self._instance.add(self.filesrc.get_element())
            self._instance.add(self.tsdemux.get_element())
        except Exception as e:
            logger.error("While adding elements to the pipeline: %s", e)

        self.filesrc.link(self.tsdemux)
        self.tsdemux.get_element().connect("pad-added",
                                      functools.partial(self.tsdemux.on_pad_added))

        return self._instance


    def stop_pipeline(self) -> None:
        pass
