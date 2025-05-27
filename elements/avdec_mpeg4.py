from elements.base_element_wrapper import GStreamerElementWrapper


class AVDecMPEG4Wrapper(GStreamerElementWrapper):
    def __init__(self, name="avdec_mpeg4"):
        super().__init__("avdec_mpeg4", name)
