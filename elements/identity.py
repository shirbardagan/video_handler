from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst

class Identity(GStreamerElementWrapper):
    def __init__(self, name="identity"):
        super().__init__("identity", name)
