import threading

import gi

gi.require_version('Gst', '1.0')
from gi.repository import Gst
from common.base_logger import logger


class GStreamerElementWrapper:
    _instances = {}
    _lock = threading.Lock()

    def __new__(cls, *args, **kwargs):
        if cls not in cls._instances:
            with cls._lock:
                if cls not in cls._instances:
                    instance = super().__new__(cls)
                    cls._instances[cls] = instance
        return cls._instances[cls]

    def __init__(self, element_name: str, element_type: str):
        if not hasattr(self, "element"):
            with self._lock:
                self.element = Gst.ElementFactory.make(element_type, element_name)
                self.name = element_name

    def set_property(self, property_name: str, value) -> None:
        self.element.set_property(property_name, value)

    def get_property(self, property_name: str):
        return self.element.get_property(property_name)

    def link(self, other_element) -> bool:
        src_pad = self.element.get_static_pad('src')
        sink_pad = other_element.element.get_static_pad('sink')

        if not src_pad or not sink_pad:
            logger.error(f"Cannot find static pads: {self.name} or {other_element.name}")
            return False

        if src_pad.link(sink_pad) != Gst.PadLinkReturn.OK:
            logger.error(f"Failed to link {self.name} to {other_element.name}")
            return False

        logger.info(f"Successfully linked {self.name} to {other_element.name}")
        return True

    def connect(self, signal_name: str, callback, *user_data) -> None:
        self.element.connect(signal_name, callback, *user_data)

    def get_element(self) -> Gst.Element:
        """Returns the underlying Gst.Element instance."""
        return self.element
