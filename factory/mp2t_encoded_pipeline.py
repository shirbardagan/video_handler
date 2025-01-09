from pipelines.mp2t_h264_pipeline import MP2TH264StreamPipeline
from pipelines.mp2t_h265_pipeline import MP2TH265StreamPipeline
from pipelines.mp2t_pipeline import MP2TStreamPipeline
from pipelines.mpeg4i_pipeline import MPEG4IStreamPipeline
from pipelines.test_pipeline import TESTStreamPipeline
from pipelines.udp_pipeline import UDPStreamPipeline
from pipelines.rtsp_pipeline import RTSPStreamPipeline
from pipelines.v4l2_pipeline import V4L2StreamPipeline


class StreamPipelineFactory:
    _pipeline_mapping = {
        "h264": MP2TH264StreamPipeline,
        "h265": MP2TH265StreamPipeline
    }

    @classmethod
    def get_pipeline_type(cls, encode_type: str) -> MP2TStreamPipeline:
        mp2t_pipeline_class = cls._pipeline_mapping.get(encode_type)
        if mp2t_pipeline_class:
            return mp2t_pipeline_class()
        raise ValueError(f"Unsupported encoding type: {encode_type}")