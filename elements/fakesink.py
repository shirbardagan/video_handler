from elements.base_element_wrapper import GStreamerElementWrapper


class FakeSinkWrapper(GStreamerElementWrapper):
    def __init__(self, type="fakesink"):
        super().__init__(type, "fakesink")