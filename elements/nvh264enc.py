from elements.base_element_wrapper import GStreamerElementWrapper


class NVH264EncWrapper(GStreamerElementWrapper):
    def __init__(self, name="nvh264enc"):
        super().__init__("nvh264enc", name)
