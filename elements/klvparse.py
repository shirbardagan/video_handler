from elements.base_element_wrapper import GStreamerElementWrapper


class KLVParseWrapper(GStreamerElementWrapper):
    def __init__(self, name="klvparse"):
        super().__init__(name, "klvparse")