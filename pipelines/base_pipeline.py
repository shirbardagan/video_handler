from abc import ABC, abstractmethod


class BaseStreamPipeline(ABC):
    @abstractmethod
    def start_pipeline(self, data):
        pass

    @abstractmethod
    def stop_pipeline(self):
        pass