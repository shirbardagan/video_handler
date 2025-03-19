from elements import GStreamerElementWrapper


class VideoConvertWrapper(GStreamerElementWrapper):
    def __init__(self, name="videoconvert"):
        super().__init__("videoconvert", name)