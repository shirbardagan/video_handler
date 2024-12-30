from elements.base_element_wrapper import GStreamerElementWrapper


class H264ParseWrapper(GStreamerElementWrapper):
    def __init__(self, name="h264parse"):
        super().__init__(name, "h264parse")