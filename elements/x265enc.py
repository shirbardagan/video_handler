from elements.base_element_wrapper import GStreamerElementWrapper


class X265enc(GStreamerElementWrapper):
    def __init__(self, name="x265enc"):
        super().__init__("x265enc", name)

