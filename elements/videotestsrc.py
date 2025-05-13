from elements import GStreamerElementWrapper


class VideoTestSrcWrapper(GStreamerElementWrapper):
    def __init__(self, name="videotestsrc"):
        super().__init__("videotestsrc", name)
