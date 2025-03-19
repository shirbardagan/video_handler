import gi

from gi.repository import Gst

gi.require_version('Gst', '1.0')

from app_instance import app
from common.base_logger import logger
from elements import GStreamerElementWrapper


class AppSrcWrapper(GStreamerElementWrapper):
    def __init__(self, type="appsrc", name="appsrc"):
        super().__init__(type, name)


class DataAppSrc(AppSrcWrapper):
    def __init__(self, name="appsrc"):
        super().__init__("appsrc", name)


class VideoAppSrc(AppSrcWrapper):
    def __init__(self, name="appsrc"):
        super().__init__("appsrc", name)

    def on_need_data(self, _, __):
        """Callback function that triggers when appsrc is ready to recieve data."""
        if self.get_element() not in app.state.webrtc_conn_videosrc:
            app.state.webrtc_conn_videosrc.append(self.get_element())

    def link(self, other_element) -> bool:
        try:
            src_pad = self.get_element().get_static_pad("src")
            dynamic_sink_pad = other_element.get_element().request_pad_simple("sink_%u")
            if src_pad.link(dynamic_sink_pad) == Gst.PadLinkReturn.OK:
                logger.info("Successfully linked %s to %s", self.get_name(), other_element.get_name())
                return True
        except Exception as e:
            logger.error("While linking %s to %s: %s", self.get_name(), other_element.get_name(), e)
            return False
