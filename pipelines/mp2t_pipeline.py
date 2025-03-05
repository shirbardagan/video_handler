import functools

from elements import TSDemuxWrapper, FileSrcWrapper, KLVParseWrapper, DataAppSink
from pipelines.base_pipeline import BaseStreamPipeline
from common.base_logger import logger


class MP2TStreamPipeline(BaseStreamPipeline):
    _shared_instance = None

    def __init__(self):
        if MP2TStreamPipeline._shared_instance is None:
            super().__init__()
            MP2TStreamPipeline._shared_instance = self._instance

            initialized_pipeline_elements_tuple = (FileSrcWrapper(),
                                                   TSDemuxWrapper(),
                                                   KLVParseWrapper(),
                                                   DataAppSink())
            (self.filesrc, self.tsdemux, self.klvparse, self.datasink) = initialized_pipeline_elements_tuple

            elements = [self.filesrc, self.tsdemux, self.klvparse, self.datasink]
            self.has_elements_initialized(elements)

            self.tsdemux.connect("pad-added",
                                 functools.partial(self.tsdemux.on_pad_added,
                                                   self.klvparse.get_element()))
            self.filesrc.set_property("location", "/home/elbit/Desktop/flights/VNIR_ZOOM.ts")

            self.datasink.set_property("emit-signals", True)
            self.datasink.connect("new-sample", functools.partial(self.datasink.on_data_sample))

        else:
            self._instance = MP2TStreamPipeline._shared_instance



    def create_pipeline(self):
        try:
            self._instance.add(self.filesrc.get_element())
            self._instance.add(self.tsdemux.get_element())
            self._instance.add(self.klvparse.get_element())
            self._instance.add(self.datasink.get_element())
        except Exception as e:
            logger.error("While adding elements to the pipeline: %s", e)

        self.filesrc.link(self.tsdemux)
        self.klvparse.link(self.datasink)
        return self._instance


    def stop_pipeline(self) -> None:
        pass
