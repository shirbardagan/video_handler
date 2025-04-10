from elements.base_element_wrapper import GStreamerElementWrapper


class QueueWrapper(GStreamerElementWrapper):
    def __init__(self, name="queue"):
        super().__init__("queue", name)
