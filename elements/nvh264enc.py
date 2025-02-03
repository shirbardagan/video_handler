from elements.base_element_wrapper import GStreamerElementWrapper


class NVH264EncWrapper(GStreamerElementWrapper):
    def __init__(self, type="nvh264enc"):
        super().__init__(type, "nvh264enc")
