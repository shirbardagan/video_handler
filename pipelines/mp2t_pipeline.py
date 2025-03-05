import functools

from elements import TSDemuxWrapper, FileSrcWrapper, KLVParseWrapper, DataAppSink
from pipelines.base_pipeline import BaseStreamPipeline
from common.base_logger import logger

import gi
from gi.repository import Gst

gi.require_version('Gst', '1.0')


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

            self.filesrc.set_property("location", "/home/elbit/Desktop/flights/VNIR_ZOOM.ts")

            self.datasink.set_property("emit-signals", True)

        else:
            self._instance = MP2TStreamPipeline._shared_instance

    def create_pipeline(self) -> Gst.Pipeline:
        self._add_elements()
        self._connect_signals()
        self._link_elements()
        return self._instance

    def _add_elements(self):
        elements_to_add = [
            self.filesrc, self.tsdemux, self.klvparse, self.datasink
        ]
        self.add_elements(elements_to_add)

    def _connect_signals(self):
        self.tsdemux.connect("pad-added",
                             functools.partial(self.tsdemux.on_pad_added,
                                               self.klvparse.get_element()))
        self.datasink.connect("new-sample", functools.partial(self.datasink.on_data_sample))

    def _link_elements(self):
        links = [
            (self.filesrc, self.tsdemux),
            (self.klvparse, self.datasink)
        ]
        self.link_elements(links)
