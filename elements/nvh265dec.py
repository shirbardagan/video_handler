from elements.base_element_wrapper import GStreamerElementWrapper


class NVH265DecWrapper(GStreamerElementWrapper):
    def __init__(self, name="nvh265dec"):
        super().__init__(name, "nvh265dec")