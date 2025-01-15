from common.base_logger import logger
from pipelines.mp2t_h264_pipeline import MP2TH264StreamPipeline
from pipelines.mp2t_h265_pipeline import MP2TH265StreamPipeline


class MP2TFactory:
    _pipeline_mapping = {
        "video/x-h2654": MP2TH264StreamPipeline,
        "video/x-h265": MP2TH265StreamPipeline
    }

    @classmethod
    def get_pipeline_type(cls, encode_type: str):
        try:
            mp2t_pipeline_class = cls._pipeline_mapping.get(encode_type)
            return mp2t_pipeline_class()
        except Exception as e:
            logger.error("While trying to define stream encoding: %s", e)
