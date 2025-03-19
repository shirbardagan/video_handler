from elements.base_element_wrapper import GStreamerElementWrapper


class AVDecH264Wrapper(GStreamerElementWrapper):
    def __init__(self, name="avdec_h264"):
        super().__init__("avdec_h264", name)
