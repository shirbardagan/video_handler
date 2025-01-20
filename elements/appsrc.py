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

    def link(self, other_element) -> None:
        try:
            rtph264pay_src_pad = self.get_element().get_static_pad("src")
            webrtcbin_dynamic_sink_pad = other_element.get_element().request_pad_simple("sink_%u")
            if rtph264pay_src_pad.link(webrtcbin_dynamic_sink_pad) == Gst.PadLinkReturn.OK:
                logger.info("Successfully linked %s to %s", self._name, other_element._name)
        except Exception as e:
            logger.error("While linking rtph264pay with %s. %s", other_element._name, e)

    def on_need_data(self,_, __):
        app.state.OPEN_CONNECTIONS.append(self.get_element())
        # print("in need data")


class DataAppSrc(AppSrcWrapper):
    def __init__(self, type="appsrc"):
        super().__init__(type, "datasrc")


class VideoAppSrc(AppSrcWrapper):
    def __init__(self, type="appsrc"):
        super().__init__(type, "videosrc")


