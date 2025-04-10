from elements.base_element_wrapper import GStreamerElementWrapper


class FakeSinkWrapper(GStreamerElementWrapper):
    def __init__(self, name="fakesink"):
        super().__init__("fakesink", name)
