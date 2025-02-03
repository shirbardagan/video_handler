from elements.base_element_wrapper import GStreamerElementWrapper


class H264Parse1Wrapper(GStreamerElementWrapper):
    def __init__(self, type="h264parse1"):
        super().__init__(type, "h264parse1")