from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst

class AVDecH265Wrapper(GStreamerElementWrapper):
    def __init__(self, name="avdec_h265"):
        super().__init__("avdec_h265", name)
