import threading
import gi
from pycparser.c_ast import Union

gi.require_version('Gst', '1.0')
from gi.repository import Gst
from common.base_logger import logger


class GStreamerElementWrapper:
    """
    A wrapper class for GStreamer elements to simplify creation, configuration,
    linking, and signal handling.
    """

    _element_to_string: str
    _instances = {}
    _lock = threading.Lock()

    # def __new__(cls, *args, **kwargs):
    #     if getattr(cls, "allow_multiple_instances", False):
    #         return super().__new__(cls)
    #
    #     with cls._lock:
    #         if cls in cls._instances:
    #             cls._instances[cls].get_element().set_state(Gst.State.NULL)
    #             cls._instances[cls].get_element().unref()
    #         instance = super().__new__(cls)
    #         cls._instances[cls] = instance
    #     return cls._instances[cls]

    def __init__(self, element_type: str, element_name: str):
        """
        Initializes a GStreamer element.

        Args:
            element_type (str): The type of GStreamer element to create.
            element_name (str): The name to assign to the element.
        """
        self._element = Gst.ElementFactory.make(element_type, element_name)
        self._element_to_string = element_name
        self._name = element_name
        self.initialized = True

        self._instances[self] = self._element

    def set_property(self, property_name: str, value) -> None:
        """
        Sets a property on the GStreamer element.

        Args:
            property_name (str): The name of the property to set.
            value: The value to assign to the property.
        """
        self._element.set_property(property_name, value)
        self._element_to_string += f" {property_name}={value} "

    def get_property(self, property_name: str):
        """
        Retrieves a property value from the GStreamer element.

        Args:
            property_name (str): The name of the property to retrieve.
        Returns:
            The value of the requested property.
        """
        return self._element.get_property(property_name)

    def link(self, other_element) -> bool:
        """
        Links the current element to another GStreamer element.

        Args:
            other_element (GStreamerElementWrapper): The element to link to.
        Returns:
            bool: True if linking was successful, False otherwise.
        """
        src_pad = self._element.get_static_pad('src')
        sink_pad = other_element.get_element().get_static_pad('sink')

        if not src_pad or not sink_pad:
            logger.error(f"Cannot find static pads: {self._name} or {other_element.get_name()}")
            return False

        if src_pad.link(sink_pad) != Gst.PadLinkReturn.OK:
            logger.error(f"Failed to link {self._name} to {other_element.get_name()}")
            return False

        logger.info(f"Successfully linked {self._name} to {other_element.get_name()}")
        return True

    def connect(self, signal_name: str, callback, *user_data) -> None:
        """
        Connects a signal to the GStreamer element.

        Args:
            signal_name (str): The name of the signal.
            callback (callable): The function to call when the signal is emitted.
            *user_data: Additional data to pass to the callback function.
        """
        self.get_element().connect(signal_name, callback, *user_data)

    def get_element(self) -> Gst.Element:
        """Returns the underlying GStreamer element."""
        return self._element

    def get_element_to_string(self) -> Gst.Element:
        """Returns a string representation of the GStreamer element and its properties."""
        return self._element_to_string

    def get_name(self) -> str:
        """Returns the name of the GStreamer element."""
        return self._name