import gi

gi.require_version('Gst', '1.0')
from gi.repository import Gst
from common.base_logger import logger


class GStreamerElementWrapper:
    def __init__(self, element_name: str, element_type: str):
        self.element = Gst.ElementFactory.make(element_type, element_name)
        if not self.element:
            raise ValueError(f"While creating element: {element_type}")
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
