import threading
import gi
from pycparser.c_ast import Union

gi.require_version('Gst', '1.0')
from gi.repository import Gst
from common.base_logger import logger


class GStreamerElementWrapper:
    _instances = {}
    _lock = threading.Lock()

    # def __new__(cls, *args, **kwargs):
    #     if getattr(cls, "allow_multiple_instances", False):
    #         return super(GStreamerElementWrapper, cls).__new__(cls)
    #
    #     with cls._lock:
    #         if cls not in cls._instances:
    #             instance = super().__new__(cls)
    #             cls._instances[cls] = instance
    #     return cls._instances[cls]

    def __init__(self, element_type: str, element_name: str):
        # if not hasattr(self, "initialized"):
        self._element = Gst.ElementFactory.make(element_type, element_name)
        self._name = element_name
        self.initialized = True

        self._instances[self] = self._element

    def set_property(self, property_name: str, value) -> None:
        self._element.set_property(property_name, value)

    def get_property(self, property_name: str):
        return self._element.get_property(property_name)

    def link(self, other_element) -> bool:
        src_pad = self._element.get_static_pad('src')
        sink_pad = other_element.get_element().get_static_pad('sink')

        if not src_pad or not sink_pad:
            logger.error(f"Cannot find static pads: {self._name} or {other_element._name}")
            return False

        if src_pad.link(sink_pad) != Gst.PadLinkReturn.OK:
            logger.error(f"Failed to link {self._name} to {other_element._name}")
            return False

        logger.info(f"Successfully linked {self._name} to {other_element._name}")
        return True

    def connect(self, signal_name: str, callback, *user_data) -> None:
        self.get_element().connect(signal_name, callback, *user_data)

    def get_element(self) -> Gst.Element:
        """Returns the underlying Gst.Element instance."""
        return self._element


