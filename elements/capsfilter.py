from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst

class CapsFilterWrapper(GStreamerElementWrapper):
    def __init__(self, name="capsfilter"):
        super().__init__("capsfilter", name)

    def set_property(self, property_name: str, value) -> None:
        self._element.set_property(property_name, Gst.Caps.from_string(value))
        self._element_to_string += f" {property_name}={value} "