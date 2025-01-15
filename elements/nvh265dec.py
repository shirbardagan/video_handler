from elements.base_element_wrapper import GStreamerElementWrapper


class NVH265DecWrapper(GStreamerElementWrapper):
    def __init__(self, type="nvh265dec"):
        super().__init__(type, "nvh265dec")