from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper

from gi.repository import Gst

class QueueWrapper(GStreamerElementWrapper):
    def __init__(self, name="queue"):
        super().__init__("queue", name)

