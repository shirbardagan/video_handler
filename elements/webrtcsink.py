from elements.base_element_wrapper import GStreamerElementWrapper


class WebRTCSinkWrapper(GStreamerElementWrapper):
    def __init__(self, name="webrtcsink"):
        super().__init__(name, "webrtcsink")