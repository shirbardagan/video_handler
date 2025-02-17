from elements.base_element_wrapper import GStreamerElementWrapper


class RTSPSrcWrapper(GStreamerElementWrapper):
    def __init__(self, name="rtspsrc"):
        super().__init__("rtspsrc", name)
