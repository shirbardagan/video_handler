from elements.base_element_wrapper import GStreamerElementWrapper


class X265enc(GStreamerElementWrapper):
    def __init__(self, type="x265enc"):
        super().__init__(type, "x265enc")

