from elements.base_element_wrapper import GStreamerElementWrapper


class VideoConvertWrapper(GStreamerElementWrapper):
    def __init__(self, type="videoconvert"):
        super().__init__(type, "videoconvert")