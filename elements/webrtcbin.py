from elements import GStreamerElementWrapper


class WebRTCBinWrapper(GStreamerElementWrapper):

    def __init__(self, type="webrtcbin"):
        super().__init__(type, "webrtcbin")
