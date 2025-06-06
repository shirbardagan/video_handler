import functools

from common.base_logger import logger
from config_models.config import CAPS_MPEG4I, CAPS_H264
from config_models.system_config import SystemSettingsConfig
from elements import H264ParseWrapper, RTPH264Pay, VideoAppSink, UDPSrcWrapper, \
    CapsFilterWrapper, NVMPEG4VideoDec, NVH264EncWrapper, MPEG4Filter, MPEG4VideoParse, GStreamerElementWrapper, \
    AVDecMPEG4Wrapper
from elements.queue import QueueWrapper
from pipelines import BaseStreamPipeline

import gi
from gi.repository import Gst
gi.require_version('Gst', '1.0')

system_conf = SystemSettingsConfig()


class MPEG4IStreamPipeline(BaseStreamPipeline):
    def __init__(self):
        super().__init__()
        initialized_pipeline_elements_tuple = (UDPSrcWrapper(),
                                               CapsFilterWrapper("mpeg4capsfilter"),
                                               MPEG4Filter(),
                                               MPEG4VideoParse(),
                                               self.select_mpeg4_decoder(system_conf.use_gpu),
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
            self.h264encoder.set_property("max-errors", -1)
        else:
            self.h264encoder.set_property("key-int-max", 30)
            self.h264encoder.set_property("tune", "zerolatency")
            self.h264encoder.set_property("speed-preset", "ultrafast")
            self.h264encoder.set_property("interlaced", False)

        self.h264parse.set_property("config-interval", -1)
        self.rtph264pay.set_property("config-interval", -1)

        self.rtph264pay.set_property("pt", 96)

        self.videosink.set_property("emit-signals", True)
        self.videosink.set_property("sync", False)
        self.videosink.set_property("async", False)

        self.capsfilter0.set_property("caps", CAPS_MPEG4I)
        self.capsfilter1.set_property("caps", CAPS_H264)

    def create_pipeline(self) -> Gst.Pipeline:
        self._add_elements()
        self._connect_signals()
        self._link_elements()
        return self._instance

    def _add_elements(self):
        elements_to_add = [self.udpsrc, self.mpeg4filter, self.mpeg4videoparse, self.nvmpeg4videodec, self.queue,
                           self.h264encoder, self.capsfilter0, self.capsfilter1, self.h264parse, self.rtph264pay,
                           self.videosink]

        self.add_elements(elements_to_add)

    def _connect_signals(self):
        self.videosink.connect("new-sample", functools.partial(self.videosink.on_data_sample))

    def _link_elements(self):
        links = [
            (self.udpsrc, self.capsfilter0),
            (self.capsfilter0, self.mpeg4filter),
            (self.mpeg4filter, self.mpeg4videoparse),
            (self.mpeg4videoparse, self.nvmpeg4videodec),
            (self.nvmpeg4videodec, self.queue),
            (self.queue, self.h264encoder),
            (self.h264encoder, self.capsfilter1),
            (self.capsfilter1, self.h264parse),
            (self.h264parse, self.rtph264pay),
            (self.rtph264pay, self.videosink)
        ]
        self.link_elements(links)
        pipeline_to_string = self.get_pipeline_string(links)
        logger.info("Pipeline string: %s", pipeline_to_string)

    @staticmethod
    def select_mpeg4_decoder(use_gpu: bool) -> GStreamerElementWrapper:
        """Selects an MPEG4 decoder based on GPU availability."""
        if use_gpu:
            try:
                decoder = NVMPEG4VideoDec("nvmpeg4decoder")
                if decoder.initialized:
                    return decoder
                else:
                    logger.warning("Failed initializing GPU MPEG4 decoder, replacing it with CPU MPEG4 decoder.")
            except Exception as e:
                logger.error("Failed initializing GPU MPEG4 decoder: %s", e)

        return AVDecMPEG4Wrapper("avmpeg4decoder")
