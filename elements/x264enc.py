from elements.base_element_wrapper import GStreamerElementWrapper


class X264enc(GStreamerElementWrapper):
    def __init__(self, type="x264enc"):
        super().__init__(type, "x264enc")

