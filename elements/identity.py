from elements.base_element_wrapper import GStreamerElementWrapper


class Identity(GStreamerElementWrapper):
    def __init__(self, name="identity"):
        super().__init__("identity", name)
