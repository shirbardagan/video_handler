import json
from abc import abstractmethod

from common.base_logger import logger
import gi

gi.require_version('Gst', '1.0')
gi.require_version("GstApp", "1.0")
from gi.repository import Gst, GLib, GstApp


class BaseStreamPipeline:
    # def __init__(self):
    #     # self.updsrc =
    #     # TODO: initialize udpsrc parameters
    #     pass

    @abstractmethod
    def create_pipeline(self):
        pass

    @abstractmethod
    def start_pipeline(self) -> None:
        pass

    @abstractmethod
    def stop_pipeline(self) -> None:
        pass

    @staticmethod
    def has_elements_initialized(elements):
        if not all(elements):
            logger.error("Not all elements could be created.")


class BaseSinkPipeline(BaseStreamPipeline):
    def __init__(self):
        self._instance = Gst.Pipeline.new("pipeline")

    def create_pipeline(self):
        pass

    def start_pipeline(self) -> None:
        try:
            self.create_pipeline()
            logger.info("Starting pipeline")
            ret = self._instance.set_state(Gst.State.PLAYING)
            if ret == Gst.StateChangeReturn.FAILURE:
                logger.error("Unable to set the pipeline to the playing state")
            else:
                logger.info("Pipeline is now playing")
        except Exception as e:
            logger.error("While starting pipeline: %s", e)


class BaseSrcPipeline(BaseStreamPipeline):
    def __init__(self):
        self._instance = Gst.Pipeline.new("pipeline")
