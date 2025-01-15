from elements.base_element_wrapper import GStreamerElementWrapper


class KLVParseWrapper(GStreamerElementWrapper):
    def __init__(self, type="klvparse"):
        super().__init__(type, "klvparse")
