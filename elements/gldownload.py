from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst

class GLDownloadWrapper(GStreamerElementWrapper):
    def __init__(self, name="gldownload"):
        super().__init__("gldownload", name)
