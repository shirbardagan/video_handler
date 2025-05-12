import functools

from common.base_logger import logger
from config_models.config import CAPS_MPEG4I, CAPS_H264
from config_models.system_config import SystemSettingsConfig
from elements import H264ParseWrapper, RTPH264Pay, VideoAppSink, UDPSrcWrapper, \
    CapsFilterWrapper, NVMPEG4VideoDec, NVH264EncWrapper, MPEG4Filter, MPEG4VideoParse
from elements.queue import QueueWrapper
from pipelines import BaseStreamPipeline

system_conf = SystemSettingsConfig()


class MPEG4IStreamPipeline(BaseStreamPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (UDPSrcWrapper(),
                                               CapsFilterWrapper("mpeg4capsfilter"),
                                               MPEG4Filter(),
                                               MPEG4VideoParse(),
                                               NVMPEG4VideoDec(),
                                               QueueWrapper(),
                                               self.select_h264_encoder(system_conf.use_gpu),
                                               CapsFilterWrapper("h264capsfilter"),
                                               H264ParseWrapper("h264parser"),
                                               RTPH264Pay("rtph264pay"),
                                               VideoAppSink("videosink")
                                               )

        (self.udpsrc, self.capsfilter0, self.mpeg4filter, self.mpeg4videoparse, self.nvmpeg4videodec, self.queue,
         self.h264encoder, self.capsfilter1,
         self.h264parse, self.rtph264pay, self.videosink) = initialized_pipeline_elements_tuple

        elements = [self.udpsrc, self.nvmpeg4videodec, self.h264encoder, self.capsfilter1,
                    self.capsfilter0, self.h264parse, self.rtph264pay, self.videosink]
        self.has_elements_initialized(elements)

        self.udpsrc.set_multicast_properties()

        self.h264encoder.set_property("bitrate", 4096)

        if isinstance(self.h264encoder, NVH264EncWrapper):
            self.h264encoder.set_property("preset", "low-latency-hp")
            self.h264encoder.set_property("zerolatency", True)
            self.h264encoder.set_property("gop-size", 30)
            # self.h265decoder.set_property("max-errors", -1)
        else:
            # self.h265decoder.set_property("max-errors", -1)
            self.h264encoder.set_property("key-int-max", 30)
            self.h264encoder.set_property("tune", "zerolatency")
            self.h264encoder.set_property("speed-preset", "ultrafast")

        self.h264parse.set_property("config-interval", -1)
        self.rtph264pay.set_property("config-interval", -1)

        self.rtph264pay.set_property("pt", 96)

        self.videosink.set_property("emit-signals", True)
        self.videosink.set_property("sync", False)
        self.videosink.set_property("async", False)

        self.capsfilter0.set_property("caps", CAPS_MPEG4I)
        self.capsfilter1.set_property("caps", CAPS_H264)

    def create_pipeline(self):
        try:
            self._instance.add(self.udpsrc.get_element())
            self._instance.add(self.mpeg4filter.get_element())
            self._instance.add(self.mpeg4videoparse.get_element())
            self._instance.add(self.nvmpeg4videodec.get_element())
            self._instance.add(self.queue.get_element())
            self._instance.add(self.h264encoder.get_element())
            self._instance.add(self.capsfilter0.get_element())
            self._instance.add(self.capsfilter1.get_element())
            self._instance.add(self.h264parse.get_element())
            self._instance.add(self.rtph264pay.get_element())
            self._instance.add(self.videosink.get_element())
        except Exception as e:
            logger.error("While adding elements to the pipeline: %s", e)

        self.udpsrc.link(self.capsfilter0)
        self.capsfilter0.link(self.mpeg4filter)
        self.mpeg4filter.link(self.mpeg4videoparse)

        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

        self.mpeg4videoparse.link(self.nvmpeg4videodec)
        self.nvmpeg4videodec.link(self.queue)
        self.queue.link(self.h264encoder)
        self.h264encoder.link(self.capsfilter1)
        self.capsfilter1.link(self.h264parse)
        self.h264parse.link(self.rtph264pay)
        self.rtph264pay.link(self.videosink)

        return self._instance
