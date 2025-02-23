from abc import abstractmethod

from typing_extensions import List, Tuple

from app_instance import app
from common.base_logger import logger
import gi

from elements.appsrc import VideoAppSrc
from elements.base_element_wrapper import GStreamerElementWrapper

gi.require_version('Gst', '1.0')
gi.require_version("GstApp", "1.0")
from gi.repository import Gst, GLib, GstApp


class BaseStreamPipeline:
    def __init__(self):
        self._instance = Gst.Pipeline.new("pipeline")
        self._elements = []

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
    def has_elements_initialized(elements: List[GStreamerElementWrapper]):
        if not all(elements):
            logger.error("Not all elements could be created.")

    @staticmethod
    def link_elements(links: List[Tuple[GStreamerElementWrapper, GStreamerElementWrapper]]):
        for src, sink in links:
            try:
                src.link(sink)
            except Exception as e:
                logger.error("While linking elements %s -> %s: %s", src.get_name(), sink.get_name(), e)

    def add_elements(self, elements):
        for element in elements:
            try:
                self._instance.add(element.get_element())
                self._elements.append(element)
            except Exception as e:
                logger.error("While adding %s element to the pipeline: %s", element.get_name(), e)

    def unref(self):
        try:
            for element in self._elements:
                if isinstance(element, VideoAppSrc):
                    if element in app.state.OPEN_CONNECTIONS:
                        app.state.OPEN_CONNECTIONS.remove(element)
                element.get_element().set_state(Gst.State.NULL)
                element.get_element().unref()
            self._instance.set_state(Gst.State.NULL)
            self._instance.unref()
        except Exception as e:
            logger.error("While releasing pipeline resources: %s", e)


class BaseSinkPipeline(BaseStreamPipeline):
    def __init__(self):
        super().__init__()
        # app.state.CURRENT_PIPELINE = self._instance
        self._bus = self._instance.get_bus()
        self._bus.add_watch(0, self.on_bus_message)

    @abstractmethod
    def create_pipeline(self):
        pass

    @staticmethod
    def on_bus_message(bus, msg):
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
        super().__init__()

    @abstractmethod
    def create_pipeline(self):
        pass
