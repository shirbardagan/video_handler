from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst

class AVDecH264Wrapper(GStreamerElementWrapper):
    def __init__(self, type="avdec_h264"):
        super().__init__(type, "avdec_h264")
