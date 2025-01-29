from elements.v4l2src import V4L2Wrapper
from pipelines.base_pipeline import BaseSinkPipeline


class V4L2StreamPipeline(BaseSinkPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (V4L2Wrapper()
                                               )
    def create_pipeline(self):
        pass

    def start_pipeline(self) -> None:
        pass

    def stop_pipeline(self) -> None:
        pass