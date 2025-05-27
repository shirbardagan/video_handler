from elements.base_element_wrapper import GStreamerElementWrapper


class NVMPEG4VideoDec(GStreamerElementWrapper):
    def __init__(self, name="nvmpeg4videodec"):
        super().__init__("nvmpeg4videodec", name)
