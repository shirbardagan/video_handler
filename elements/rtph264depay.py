from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst

class RTPH264DePayWrapper(GStreamerElementWrapper):
    def __init__(self, name="rtph264depay"):
        super().__init__("rtph264depay", name)

