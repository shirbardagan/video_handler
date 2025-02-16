import json
import time

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

    def on_data_sample(self, appsink) -> Gst.FlowReturn:
        try:
            sample = appsink.pull_sample()
            buffer = sample.get_buffer()

            print("In on_data_sample", len(app.state.OPEN_CONNECTIONS))
            for appsrc in app.state.OPEN_CONNECTIONS_DATA:
                buffer.dts = 0
                buffer.pts = appsrc.get_element().get_clock().get_time()

                appsrc.get_element().emit("push-sample", sample)
        except Exception as e:
            logger.error("In data_sample: %s", e)
        return Gst.FlowReturn.OK



class VideoAppSink(AppSinkWrapper):
    first_time = True

    def __init__(self, type="appsink"):
        super().__init__(type, "videosink")

    def on_data_sample(self, appsink) -> Gst.FlowReturn:
        print("on data sample")
        try:
            sample = appsink.pull_sample()
            buffer = sample.get_buffer()

            if self.first_time:
                print("In on_data_sample", len(app.state.OPEN_CONNECTIONS))
                self.first_time = False
            for appsrc in app.state.OPEN_CONNECTIONS:
                buffer.dts = 0
                # buffer.pts = 0
                buffer.pts = appsrc.get_element().get_clock().get_time()

                appsrc.get_element().emit("push-sample", sample)
        except Exception as e:
            logger.error("In data_sample: %s", e)
        return Gst.FlowReturn.OK
