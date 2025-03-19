from elements import GStreamerElementWrapper


class RTPH264DePayWrapper(GStreamerElementWrapper):
    def __init__(self, name="rtph264depay"):
        super().__init__("rtph264depay", name)
