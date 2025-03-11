from app_instance import app
from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper

import gi

gi.require_version('Gst', '1.0')
from gi.repository import Gst


class AppSrcWrapper(GStreamerElementWrapper):
    allow_multiple_instances = True

    def __init__(self, type="appsrc", name="appsrc"):
        super().__init__(type, name)
        self.blocked = False



    def on_enough_data(self, _):
        app.state.PUSH_SAMPLE = False
        element = self.get_element()
        current_level = element.get_current_level_bytes()
        max_level = element.get_max_bytes()


class DataAppSrc(AppSrcWrapper):
    def __init__(self, name="appsrc"):
        super().__init__("appsrc", name)



class VideoAppSrc(AppSrcWrapper):
    def __init__(self, name="appsrc"):
        super().__init__("appsrc", name)

    def on_need_data(self, _, __):
        if self.get_element() not in app.state.webrtc_conn_videosrc:
            app.state.webrtc_conn_videosrc.append(self.get_element())

    def link(self, other_element) -> None:
        try:
            rtph264pay_src_pad = self.get_element().get_static_pad("src")
            webrtcbin_dynamic_sink_pad = other_element.get_element().request_pad_simple("sink_%u")
            if rtph264pay_src_pad.link(webrtcbin_dynamic_sink_pad) == Gst.PadLinkReturn.OK:
                logger.info("Successfully linked %s to %s", self.get_name(), other_element.get_name())
        except Exception as e:
            logger.error("While linking identity with %s. %s", other_element.get_name(), e)