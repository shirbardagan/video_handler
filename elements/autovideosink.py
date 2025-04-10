from elements.base_element_wrapper import GStreamerElementWrapper


class AutoVideoSinkWrapper(GStreamerElementWrapper):
    def __init__(self, name="autovideosink"):
        super().__init__("autovideosink", name)
