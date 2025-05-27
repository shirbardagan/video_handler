from elements.base_element_wrapper import GStreamerElementWrapper


class MPEG4VideoParse(GStreamerElementWrapper):
    def __init__(self, name="mpeg4videoparse"):
        super().__init__("mpeg4videoparse", name)
