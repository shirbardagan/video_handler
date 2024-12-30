from elements.base_element_wrapper import GStreamerElementWrapper


class AppSinkWrapper(GStreamerElementWrapper):
    def __init__(self, name="tsdemux"):
        super().__init__(name, "tsdemux")