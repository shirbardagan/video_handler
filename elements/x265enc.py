from elements import GStreamerElementWrapper


class X265enc(GStreamerElementWrapper):
    def __init__(self, name="x265enc"):
        super().__init__("x265enc", name)

