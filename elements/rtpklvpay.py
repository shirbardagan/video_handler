from elements import GStreamerElementWrapper


class RTPKLVPayWrapper(GStreamerElementWrapper):
    def __init__(self, name="rtpklvpay"):
        super().__init__(name, "rtpklvpay")
