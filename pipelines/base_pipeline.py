from abc import ABC, abstractmethod


class BaseStreamPipeline(ABC):
    @abstractmethod
    def start_pipeline(self, data) -> None:
        pass

    @abstractmethod
    def stop_pipeline(self) -> None:
        pass