from elements.base_element_wrapper import GStreamerElementWrapper


class H265ParseWrapper(GStreamerElementWrapper):
    def __init__(self, type="h265parse"):
        super().__init__(type, "h265parse")

