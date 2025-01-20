import json

from app_instance import app
from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper
import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst


class AppSinkWrapper(GStreamerElementWrapper):
    def __init__(self, type="appsink", name="appsink"):
        super().__init__(type, name)


class DataAppSink(AppSinkWrapper):
    def __init__(self, type="appsink"):
        super().__init__(type, "datasink")


class VideoAppSink(AppSinkWrapper):
    def __init__(self, type="appsink"):
        super().__init__(type, "videosink")

    def on_data_sample(self, appsink) -> Gst.FlowReturn:
        try:
            sample = appsink.pull_sample()
            for appsrc in app.state.OPEN_CONNECTIONS:
                appsrc.push_sample(sample)
        except Exception as e:
            logger.error("In data_sample: %s", e)
        return Gst.FlowReturn.OK
