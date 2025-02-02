import json
from abc import abstractmethod

from common.base_logger import logger
import gi

gi.require_version('Gst', '1.0')
gi.require_version("GstApp", "1.0")
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
    def start_pipeline(self) -> None:
        pass

    @abstractmethod
    def stop_pipeline(self) -> None:
        pass

    @staticmethod
    def has_elements_initialized(elements):
        if not all(elements):
            logger.error("Not all elements could be created.")


class BaseSinkPipeline(BaseStreamPipeline):
    def __init__(self):
        self._instance = Gst.Pipeline.new("pipeline")
        self._bus = self._instance.get_bus()
        self._bus.add_watch(0, self.on_bus_message)

    def on_bus_message(self, bus, msg):
        if msg.type == Gst.MessageType.STATE_CHANGED:
            print(msg.parse_state_changed())
        elif msg.type == Gst.MessageType.ERROR:
            print(msg.parse_error())
        elif msg.type == Gst.MessageType.INFO:
            print(msg.parse_info())
        elif msg.type == Gst.MessageType.WARNING:
            print(msg.parse_warning())
        elif msg.type == Gst.MessageType.ELEMENT:
            structure = msg.get_structure()
        return True

    def create_pipeline(self):
        pass

    def start_pipeline(self) -> None:
        try:
            self.create_pipeline()
            logger.info("Starting pipeline")
            ret = self._instance.set_state(Gst.State.PLAYING)
            if ret == Gst.StateChangeReturn.FAILURE:
                logger.error("Unable to set the pipeline to the playing state")
            else:
                logger.info("Pipeline is now playing")
        except Exception as e:
            logger.error("While starting pipeline: %s", e)


class BaseSrcPipeline(BaseStreamPipeline):
    def __init__(self):
        self._instance = Gst.Pipeline.new("pipeline")
