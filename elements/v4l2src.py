from elements.base_element_wrapper import GStreamerElementWrapper


class V4L2SrcWrapper(GStreamerElementWrapper):
    def __init__(self, type="v4l2src"):
        super().__init__(type, "v4l2src")
