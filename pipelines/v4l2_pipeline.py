from pipelines.base_pipeline import BaseStreamPipeline


class V4L2StreamPipeline(BaseStreamPipeline):
    def create_pipeline(self):
        pass

    def start_pipeline(self, data) -> None:
        pass

    def stop_pipeline(self) -> None:
        pass