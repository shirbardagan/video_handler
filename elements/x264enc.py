from elements.base_element_wrapper import GStreamerElementWrapper


class X264enc(GStreamerElementWrapper):
    def __init__(self, name="x264enc"):
        super().__init__(name, "x264enc")

