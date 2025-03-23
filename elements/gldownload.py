from elements.base_element_wrapper import GStreamerElementWrapper


class GLDownloadWrapper(GStreamerElementWrapper):
    def __init__(self, name="gldownload"):
        super().__init__("gldownload", name)
