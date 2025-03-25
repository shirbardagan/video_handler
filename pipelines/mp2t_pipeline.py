import functools

from app_instance import app
from elements import TSDemuxWrapper, KLVParseWrapper, DataAppSink, UDPSrcWrapper

from pipelines.base_pipeline import BaseStreamPipeline
from common.base_logger import logger

import gi
from gi.repository import Gst

gi.require_version('Gst', '1.0')


class MP2TStreamPipeline(BaseStreamPipeline):
    _shared_instance = None

    def __init__(self):
        if type(self) is MP2TStreamPipeline:
            super().__init__()
            MP2TStreamPipeline._shared_instance = self._instance
            self.klv_enabled = app.state.request_data.klv.enable

            self.udpsrc = UDPSrcWrapper()
            self.tsdemux = TSDemuxWrapper()
            self.klvparse = KLVParseWrapper() if self.klv_enabled else None
            self.datasink = DataAppSink()


            self._elements = [self.udpsrc, self.tsdemux, self.datasink]
            if self.klvparse:
                self._elements.insert(2, self.klvparse)

            self.has_elements_initialized(self._elements)

            self.udpsrc.set_multicast_properties()

            self.datasink.set_property("emit-signals", True)
        else:
            self._instance = MP2TStreamPipeline._shared_instance

    def create_pipeline(self) -> Gst.Pipeline:
        self._add_elements()
        self._connect_signals()
        self._link_elements()
        return self._instance

    def _add_elements(self):
        elements_to_add = [self.udpsrc, self.tsdemux, self.datasink]

        if self.klv_enabled and self.klvparse:
            elements_to_add.insert(2, self.klvparse)

        self.add_elements(elements_to_add)

    def _connect_signals(self):
        self.tsdemux.connect("pad-added",
                             functools.partial(self.tsdemux.on_pad_added,
                                               self.klvparse.get_element() if self.klvparse else self.datasink.get_element()))
        self.datasink.connect("new-sample", functools.partial(self.datasink.on_data_sample))

    def _link_elements(self):
        links = [
            (self.udpsrc, self.tsdemux)
        ]

        if self.klvparse:
            links.append((self.klvparse, self.datasink))
        else:
            links.append((self.tsdemux, self.datasink))
        self.link_elements(links)
        pipeline_to_string = self.get_pipeline_string(links)
        logger.info("Pipeline string: %s", pipeline_to_string)
