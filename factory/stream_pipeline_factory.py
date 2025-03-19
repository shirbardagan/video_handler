from common.base_logger import logger
from pipelines.base_pipeline import BaseStreamPipeline
from pipelines.mp2t_pipeline import MP2TStreamPipeline
from pipelines.mpeg4i_pipeline import MPEG4IStreamPipeline
from pipelines.test_pipeline import TESTStreamPipeline
from pipelines.rtsp_pipeline import RTSPStreamPipeline
from pipelines.rtp_pipeline import RTPStreamPipeline
from pipelines.v4l2_pipeline import V4L2StreamPipeline
from models.play_command.request.base_stream import StreamType


class StreamPipelineFactory:
    _pipeline_mapping = {
        StreamType.RTSP: RTSPStreamPipeline,
        StreamType.TEST: TESTStreamPipeline,
        StreamType.RTP: RTPStreamPipeline,
        StreamType.V4L2: V4L2StreamPipeline,
        StreamType.MPEG4I: MPEG4IStreamPipeline,
        StreamType.MP2T: MP2TStreamPipeline,
    }

    @classmethod
    def get_pipeline_type(cls, stream_type: str) -> BaseStreamPipeline:
        try:
            stream_type_enum = StreamType(stream_type)
            pipeline_class = cls._pipeline_mapping.get(stream_type_enum)
            return pipeline_class()
        except Exception as e:
            logger.error("While trying to define pipeline type: %s", e)
