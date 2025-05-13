from elements import GStreamerElementWrapper


class X264enc(GStreamerElementWrapper):
    def __init__(self, name="x264enc"):
        super().__init__("x264enc", name)
