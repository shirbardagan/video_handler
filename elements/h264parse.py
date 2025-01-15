from elements.base_element_wrapper import GStreamerElementWrapper


class H264ParseWrapper(GStreamerElementWrapper):
    def __init__(self, type="h264parse"):
        super().__init__(type, "h264parse")