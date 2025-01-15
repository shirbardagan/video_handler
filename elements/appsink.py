from elements.base_element_wrapper import GStreamerElementWrapper


class AppSinkWrapper(GStreamerElementWrapper):
    def __init__(self, type="appsink"):
        super().__init__(type, "appsink")


class DataAppSink(AppSinkWrapper):
    def __init__(self, type="appsink"):
        super().__init__(type, "appsink")


class VideoAppSink(AppSinkWrapper):
    def __init__(self, type="appsink"):
        super().__init__(type, "appsink")
