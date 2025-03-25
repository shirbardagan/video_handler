import gi

from gi.repository import Gst

gi.require_version('Gst', '1.0')

import json

from app_instance import app
from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper


class AppSinkWrapper(GStreamerElementWrapper):
    def __init__(self, type="appsink", name="appsink"):
        super().__init__(type, name)


class DataAppSink(AppSinkWrapper):
    """A subclass of AppSinkWrapper for handling JSON data received through a datasink."""
    def __init__(self, name="appsink"):
        super().__init__("appsink", name)

    @staticmethod
    def on_data_sample(appsink) -> Gst.FlowReturn:
        """
        Callback function triggered when a new sample is received in the datasink.

        Args:
            appsink (Gst.Element): The GStreamer appsink element.
        Returns:
            Gst.FlowReturn: GST_FLOW_OK if the operation is successful.
        """
        if app.state.request_data.klv.enable:
            try:
                sample = appsink.pull_sample()
                buffer = sample.get_buffer()

                succ, info = buffer.map(Gst.MapFlags.READ)
                if not succ:
                    logger.error("Failed to map buffer")
                    return Gst.FlowReturn.ERROR
                buffer.unmap(info)

                json_data = json.loads(info.data.decode('utf-8'))
                json_string = json.dumps(json_data, indent=4)

                msg = {"event": "video_data", "data": json_string}
                json_msg = json.dumps(msg)

                channels = getattr(app.state, "channels", [])
                if channels:
                    for channel in channels:
                        channel.emit("send-string", json_msg)
            except Exception as e:
                logger.error("When extracting data sample from data sink: %s", e)
        return Gst.FlowReturn.OK


class VideoAppSink(AppSinkWrapper):
    """A subclass of AppSinkWrapper for handling video samples received through an appsink."""
    def __init__(self, name="appsink"):
        super().__init__("appsink", name)

    @staticmethod
    def on_data_sample(appsink) -> Gst.FlowReturn:
        """
        Callback function triggered when a new video sample is received in the videosink.

        Args:
            appsink (Gst.Element): The GStreamer appsink element.
        Returns:
            Gst.FlowReturn: GST_FLOW_OK if the operation is successful.
        """
        try:
            if appsink:
                sample = appsink.pull_sample()
                buffer = sample.get_buffer()

                for appsrc in app.state.webrtc_conn_videosrc:
                    if appsrc:
                        if buffer:
                            buffer.dts = 0
                            buffer.pts = 0
                        appsrc.emit("push-sample", sample)
        except Exception as e:
            logger.error("When extracting data sample from video sink: %s", e)
        return Gst.FlowReturn.OK
