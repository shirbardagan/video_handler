from elements.base_element_wrapper import GStreamerElementWrapper


class H265ParseWrapper(GStreamerElementWrapper):
    def __init__(self, name="h265parse"):
        super().__init__(name, "h265parse")

