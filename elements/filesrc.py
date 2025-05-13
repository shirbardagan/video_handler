from elements.base_element_wrapper import GStreamerElementWrapper


class FileSrcWrapper(GStreamerElementWrapper):
    def __init__(self, name="filesrc"):
        super().__init__("filesrc", name)
