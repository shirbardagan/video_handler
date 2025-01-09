from elements.base_element_wrapper import GStreamerElementWrapper


class AppSrcWrapper(GStreamerElementWrapper):
    def __init__(self, name="appsrc"):
        super().__init__(name, "appsrc")