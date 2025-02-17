from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst

class AVDecH264Wrapper(GStreamerElementWrapper):
    def __init__(self, name="avdec_h264"):
        super().__init__("avdec_h264", name)
