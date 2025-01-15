from elements.base_element_wrapper import GStreamerElementWrapper


class VideoTestSrcWrapper(GStreamerElementWrapper):
    def __init__(self, type="videotestsrc"):
        super().__init__("videotestsrc", type)