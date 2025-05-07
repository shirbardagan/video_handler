from elements.base_element_wrapper import GStreamerElementWrapper


class TimeOverlayWrapper(GStreamerElementWrapper):
    def __init__(self, name="timeoverlay"):
        super().__init__("timeoverlay", name)
