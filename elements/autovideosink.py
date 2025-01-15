from elements.base_element_wrapper import GStreamerElementWrapper


class AutoVideoSinkWrapper(GStreamerElementWrapper):
    def __init__(self, type="autovideosink"):
        super().__init__(type, "autovideosink")