from elements.base_element_wrapper import GStreamerElementWrapper


class RTPH264Pay(GStreamerElementWrapper):
    def __init__(self, name="rtph264pay"):
        super().__init__(name, "rtph264pay")