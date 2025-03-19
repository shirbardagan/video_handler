from elements import GStreamerElementWrapper


class TSParse(GStreamerElementWrapper):
    def __init__(self, name="tsparse"):
        super().__init__("tsparse", name)
