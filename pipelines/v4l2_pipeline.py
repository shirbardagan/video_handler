import functools

from common.base_logger import logger
from elements import VideoConvertWrapper, CapsFilterWrapper, H264ParseWrapper, RTPH264Pay, VideoAppSink, V4L2SrcWrapper
from elements.nvh264enc import NVH264EncWrapper
from pipelines.base_pipeline import BaseSinkPipeline

import gi
from gi.repository import Gst

gi.require_version('Gst', '1.0')


class V4L2StreamPipeline(BaseSinkPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (V4L2SrcWrapper(),
                                               VideoConvertWrapper(),
                                               NVH264EncWrapper(),
                                               CapsFilterWrapper("capsfilter1"),
                                               H264ParseWrapper(),
                                               RTPH264Pay(),
                                               VideoAppSink()
                                               )

        (self.v4l2src, self.videoconvert, self.nvh264enc, self.capsfilter, self.h264parse,
         self.rtph264pay, self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.v4l2src, self.videoconvert, self.nvh264enc, self.capsfilter, self.h264parse,
                    self.rtph264pay, self.videosink]

        super().has_elements_initialized(elements)

        self.videosink.set_property("emit-signals", True)

        self.rtph264pay.set_property("config-interval", -1)

        self.capsfilter.set_property("caps", Gst.Caps.from_string(
            "video/x-h264, stream-format=(string)byte-stream, alignment=(string)au, level=(string)4, profile=(string)main"))

    def create_pipeline(self):
        try:
            self._instance.add(self.v4l2src.get_element())
            self._instance.add(self.videoconvert.get_element())
            self._instance.add(self.nvh264enc.get_element())
            self._instance.add(self.capsfilter.get_element())
            self._instance.add(self.h264parse.get_element())
            self._instance.add(self.rtph264pay.get_element())
            self._instance.add(self.videosink.get_element())
        except Exception as e:
            logger.error("While adding elements to the pipeline: %s", e)

        self.v4l2src.link(self.videoconvert)
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

        self.videoconvert.link(self.nvh264enc)
        self.nvh264enc.link(self.capsfilter)
        self.capsfilter.link(self.h264parse)
        self.h264parse.link(self.rtph264pay)
        self.rtph264pay.link(self.videosink)

        return self._instance

    def start_pipeline(self) -> None:
        pass

    def stop_pipeline(self) -> None:
        pass
