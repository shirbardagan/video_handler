import functools
from collections import namedtuple

from elements import TSDemuxWrapper, FileSrcWrapper, KLVParseWrapper, DataAppSink, UDPSrcWrapper

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

            initialized_pipeline_elements_tuple = (UDPSrcWrapper(),
                                                   TSDemuxWrapper(),
                                                   KLVParseWrapper(),
                                                   DataAppSink())
            (self.udpsrc, self.tsdemux, self.klvparse, self.datasink) = initialized_pipeline_elements_tuple

            elements = [self.udpsrc, self.tsdemux, self.klvparse, self.datasink]
            self.has_elements_initialized(elements)

            AddressTuple = namedtuple("AddressTuple", ["ip", "port", "iface"])
            addr = AddressTuple(ip="239.3.43.3", port=6146, iface="lo")
            # addr = AddressTuple(ip="239.10.1.102", port=6022, iface="lo")
            self.udpsrc.set_properties(addr)

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
            self.udpsrc, self.tsdemux, self.klvparse, self.datasink
        ]
        self.add_elements(elements_to_add)

    def _connect_signals(self):
        self.tsdemux.connect("pad-added",
                             functools.partial(self.tsdemux.on_pad_added,
                                               self.klvparse.get_element()))
        self.datasink.connect("new-sample", functools.partial(self.datasink.on_data_sample))

    def _link_elements(self):
        links = [
            (self.udpsrc, self.tsdemux),
            (self.klvparse, self.datasink)
        ]
        self.link_elements(links)
        pipeline_to_string = self.get_pipeline_string(links)
        logger.info("Pipeline string: %s", pipeline_to_string)
