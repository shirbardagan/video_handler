from elements.base_element_wrapper import GStreamerElementWrapper


class MPEG4Filter(GStreamerElementWrapper):
    def __init__(self, name="mpeg4filter"):
        super().__init__("mpeg4filter", name)
