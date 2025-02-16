from elements.base_element_wrapper import GStreamerElementWrapper


class RTPKLVPayWrapper(GStreamerElementWrapper):
    def __init__(self, name="rtpklvpay"):
        super().__init__(name, "rtpklvpay")