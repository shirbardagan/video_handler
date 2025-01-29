from elements.base_element_wrapper import GStreamerElementWrapper


class LibDe265DecWrapper(GStreamerElementWrapper):
    def __init__(self, type="libde265dec"):
        super().__init__(type, "libde265dec")
