from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst

class TSParse(GStreamerElementWrapper):
    def __init__(self, type="tsparse"):
        super().__init__(type, "tsparse")
