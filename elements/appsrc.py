from elements.base_element_wrapper import GStreamerElementWrapper


class AppSrcWrapper(GStreamerElementWrapper):
    def __init__(self, type="appsrc"):
        super().__init__(type, "appsrc")


class DataAppSink(AppSrcWrapper):
    def __init__(self, type="appsrc"):
        super().__init__(type, "appsrc")


class VideoAppSink(AppSrcWrapper):
    def __init__(self, type="appsrc"):
        super().__init__(type, "appsrc")
