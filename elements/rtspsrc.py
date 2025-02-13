from elements.base_element_wrapper import GStreamerElementWrapper


class RTSPSrcWrapper(GStreamerElementWrapper):
    def __init__(self, type="rtspsrc"):
        super().__init__(type, "rtspsrc")
