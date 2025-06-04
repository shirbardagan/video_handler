from elements.base_element_wrapper import GStreamerElementWrapper


class KLVParseWrapper(GStreamerElementWrapper):
    def __init__(self, name="myfilter"):
        super().__init__("myfilter", name)
