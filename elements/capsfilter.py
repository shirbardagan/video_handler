from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst


class CapsFilterWrapper(GStreamerElementWrapper):
    def __init__(self, name="capsfilter"):
        super().__init__("capsfilter", name)

    def set_property(self, property_name: str, value) -> bool:
        try:
            self._element.set_property(property_name, Gst.Caps.from_string(value))
            self._element_to_string += f" {property_name}={value} "
            return True
        except Exception as e:
            logger.error(f"Failed to set property %s=%s: %s", property_name, value, e)
            return False
