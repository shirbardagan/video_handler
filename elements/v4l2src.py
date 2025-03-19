from elements import GStreamerElementWrapper


class V4L2SrcWrapper(GStreamerElementWrapper):
    def __init__(self, name="v4l2src"):
        super().__init__("v4l2src", name)
