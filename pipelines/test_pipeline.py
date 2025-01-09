from pipelines.base_pipeline import BaseStreamPipeline, BaseSinkPipeline


class TESTStreamPipeline(BaseSinkPipeline):
    def create_pipeline(self):
        pass

    def start_pipeline(self, data) -> None:
        pass

    def stop_pipeline(self) -> None:
        pass

