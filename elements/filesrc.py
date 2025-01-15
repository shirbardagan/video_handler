from elements.base_element_wrapper import GStreamerElementWrapper


class FileSrcWrapper(GStreamerElementWrapper):
    def __init__(self, type="filesrc"):
        super().__init__(type, "filesrc")