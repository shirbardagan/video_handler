from elements import RTSPSrcWrapper, VideoAppSink
from pipelines.base_pipeline import BaseSinkPipeline


class RTSPStreamPipeline(BaseSinkPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (RTSPSrcWrapper(),
                                               VideoAppSink()
                                               )

        (self.rtspsrc, self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.rtspsrc, self.videosink]

        super().has_elements_initialized(elements)

        self.videosink.set_property("emit-signals", True)
        # self.x264enc.set_property("tune", "zerolatency")
        self.videosink.set_property("sync", 0)

    def create_pipeline(self):
        pass

    def start_pipeline(self) -> None:
        pass

    def stop_pipeline(self) -> None:
        pass