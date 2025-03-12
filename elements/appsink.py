import asyncio
import base64
import json

from starlette.websockets import WebSocketDisconnect

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
    def __init__(self, name="appsink"):
        super().__init__("appsink", name)

    @staticmethod
    def on_data_sample(appsink) -> Gst.FlowReturn:
        try:
            sample = appsink.pull_sample()
            buffer = sample.get_buffer()

            succ, info = buffer.map(Gst.MapFlags.READ)
            buffer.unmap(info)

            json_data = json.loads(info.data.decode('utf-8'))
            json_string = json.dumps(json_data, indent=4)

            msg = {"event": "video_data", "data": json_string}

            json_msg = json.dumps(msg)
            if hasattr(app.state, "channels") and app.state.channels is not None:
                for channel in app.state.channels:
                    channel.emit("send-string", json_msg)

        except Exception as e:
            logger.error("In data_sample of data sink: %s", e)
        return Gst.FlowReturn.OK


class VideoAppSink(AppSinkWrapper):
    first_time = True

    def __init__(self, name="appsink"):
        super().__init__("appsink", name)

    def on_data_sample(self, appsink) -> Gst.FlowReturn:
        try:
            if appsink:
                sample = appsink.pull_sample()
                buffer = sample.get_buffer()

                for appsrc in app.state.webrtc_conn_videosrc:
                    if appsrc:
                        buffer.dts = 0
                        buffer.pts = appsrc.get_clock().get_time()
                        appsrc.emit("push-sample", sample)
        except Exception as e:
            logger.error("In data_sample of video sink: %s", e)
        return Gst.FlowReturn.OK
