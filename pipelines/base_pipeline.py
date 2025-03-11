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
        """Initializes the GStreamer pipeline and sets up the message bus."""
        self._instance = Gst.Pipeline.new("pipeline")
        self._elements = []
        self._bus = self._instance.get_bus()
        self._bus.add_watch(0, self.on_bus_message)

    @abstractmethod
    def create_pipeline(self) -> Gst.Pipeline:
        """
        Abstract method to be implemented by subclasses to define the pipeline structure.
        """
        pass

    @staticmethod
    def on_bus_message(bus: Gst.Bus, msg: Gst.Message) -> bool:
        """
        Callback function for handling GStreamer bus messages.

        Args:
            bus: The GStreamer bus instance.
            msg: The message received from the bus.
        Returns:
            True to continue receiving messages, False to stop.
        """
        # if msg.type == Gst.MessageType.STATE_CHANGED:
        #     print(msg.parse_state_changed())
        # elif msg.type == Gst.MessageType.ERROR:
        #     logger.error(msg.parse_error())
        # elif msg.type == Gst.MessageType.INFO:
        #     print(msg.parse_info())
        # elif msg.type == Gst.MessageType.WARNING:
        #     print(msg.parse_warning())
        # elif msg.type == Gst.MessageType.ELEMENT:
        #     structure = msg.get_structure()
        return True

    @staticmethod
    def has_elements_initialized(elements: List[GStreamerElementWrapper]) -> bool:
        """
        Checks if all elements in the pipeline are successfully initialized.

        Args:
            elements: List of GStreamer elements.
        Logs an error if any element is not initialized.
        """
        if not all(elements):
            logger.error("Not all elements could be created.")
            return False
        return True

    @staticmethod
    def link_elements(links: List[Tuple[GStreamerElementWrapper, GStreamerElementWrapper]]) -> bool:
        """
        Links the provided GStreamer elements together.

        Args:
            links: A list of tuples where each tuple contains a source and a sink element.
        Logs an error if linking fails.
        """
        for src, sink in links:
            try:
                src.link(sink)
            except Exception as e:
                logger.error("While linking elements %s -> %s: %s", src.get_name(), sink.get_name(), e)
                return False
        return True

    def add_elements(self, elements: List[GStreamerElementWrapper]) -> None:
        """
        Adds elements to the GStreamer pipeline.

        Args:
            elements: List of elements to add to the pipeline.
        Logs an error if an element cannot be added.
        """
        for element in elements:
            try:
                self._instance.add(element.get_element())
                self._elements.append(element)
            except Exception as e:
                logger.error("While adding %s element to the pipeline: %s", element.get_name(), e)


    def unref(self) -> bool:
        """
        Releases pipeline resources and sets all elements to NULL state.

        If an element is an instance of VideoAppSrc and exists in `app.state.OPEN_CONNECTIONS`,
        it is removed from the open connections list before being released.
        Logs an error if resource cleanup fails.
        """
        try:
            self._instance.set_state(Gst.State.NULL)
            for element in self._elements:
                element.get_element().set_state(Gst.State.NULL)
                element.get_element().unref()
            self._instance.unref()
            return True
        except Exception as e:
            logger.error("While releasing pipeline resources: %s", e)
            return False

    @staticmethod
    def get_pipeline_string(links: List[Tuple[GStreamerElementWrapper, GStreamerElementWrapper]]) -> str:
        """
        Generates a string representation of the GStreamer pipeline.

        Args:
            links: A list of tuples where each tuple contains a source and a sink element.
        Returns: The GStreamer pipeline string representation.
        """
        pipeline_parts = []

        for element1, element2 in links:
            pipeline_parts.append(element1.get_element_to_string())
            pipeline_parts.append("!")

        pipeline_parts.append(links[-1][1].get_element_to_string())
        return " ".join(pipeline_parts)

    def get_pipeline_elements(self):
        return self._elements