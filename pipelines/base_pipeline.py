import json
from abc import ABC, abstractmethod

from common.base_logger import logger
from elements import AppSinkWrapper
from gi.repository import Gst, GLib, GstApp


class BaseStreamPipeline:
    # def __init__(self):
    #     # self.updsrc =
    #     # TODO: initialize udpsrc parameters
    #     pass

    @abstractmethod
    def create_pipeline(self):
        pass

    @abstractmethod
    def start_pipeline(self, data) -> None:
        pass

    @abstractmethod
    def stop_pipeline(self) -> None:
        pass


class BaseSinkPipeline(BaseStreamPipeline):
    def on_data_sample(self, appsink: Gst.Element) -> Gst.FlowReturn:
        try:
            sample = appsink.pull_sample()
            buffer = sample.get_buffer()
            succ, info = buffer.map(Gst.MapFlags.READ)
            buffer.unmap(info)
            json_data = json.loads(info.data.decode('utf-8'))
            # self.data_queue.put(json_data)
        except Exception as e:
            logger.error("In in_data_sample: %s", e)
        return Gst.FlowReturn.OK


class BaseSrcPipeline(BaseStreamPipeline):
    pass