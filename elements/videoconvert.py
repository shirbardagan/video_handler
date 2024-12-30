from elements.base_element_wrapper import GStreamerElementWrapper


class VideoConvertWrapper(GStreamerElementWrapper):
    def __init__(self, name="videoconvert"):
        super().__init__(name, "videoconvert")