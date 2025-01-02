import asyncio
import functools
from gi.repository import Gst, GLib, GstApp, GObject, GstWebRTC

from common.base_logger import logger
import gi

gi.require_version('Gst', '1.0')
from gi.repository import Gst

gi.require_version('GstWebRTC', '1.0')
from gi.repository import GstWebRTC

gi.require_version('GstSdp', '1.0')
from gi.repository import GstSdp

from elements import (
    FileSrcWrapper,
    TSDemuxWrapper,
    H265ParseWrapper,
    NVH265DecWrapper,
    H264ParseWrapper,
    WebRTCBinWrapper,
    RTPH264Pay,
    X264enc
)

loop = GObject.MainLoop()
def create_pipeline():
    Gst.init(None)
    pipeline = Gst.Pipeline.new("pipeline")
    initialized_pipeline_elements_tuple = (FileSrcWrapper("filesrc"),
                                           TSDemuxWrapper("tsdemux"),
                                           H265ParseWrapper("h265parse"),
                                           NVH265DecWrapper("nvh265dec"),
                                           X264enc("x264enc"),
                                           H264ParseWrapper("h264parse"),
                                           RTPH264Pay("rtph264Pay"),
                                           WebRTCBinWrapper("webrtcbin")
                                           )

    filesrc, tsdemux, h265parse, nvh265dec, x264enc, h264parse, rtph264pay, webrtcbin = initialized_pipeline_elements_tuple

    if not all([filesrc.get_element(), tsdemux.get_element(), h265parse.get_element(), nvh265dec.get_element(),
               webrtcbin.get_element()]):
        logger.error("Not all elements could be created.")

    filesrc.set_property("location", "/home/elbit/Desktop/flights/VNIR_ZOOM.ts")

    pipeline.add(filesrc.get_element())
    pipeline.add(tsdemux.get_element())
    pipeline.add(h265parse.get_element())
    pipeline.add(nvh265dec.get_element())
    pipeline.add(x264enc.get_element())
    pipeline.add(h264parse.get_element())
    pipeline.add(rtph264pay.get_element())
    pipeline.add(webrtcbin.get_element())

    filesrc.link(tsdemux)
    tsdemux.connect("pad-added", functools.partial(tsdemux.on_pad_added, elements=h265parse.get_element()))
    h265parse.link(nvh265dec)
    nvh265dec.link(x264enc)
    x264enc.link(h264parse)
    h264parse.link(rtph264pay)
    rtph264pay.link(webrtcbin)

    return pipeline

def play(pipe):
    ret = pipe.set_state(Gst.State.PLAYING)
    if ret == Gst.StateChangeReturn.FAILURE:
        logger.error("Failed to set pipeline to PLAYING state.")
    elif ret == Gst.StateChangeReturn.ASYNC:
        logger.info("Pipeline state change is happening asynchronously.")
    elif ret == Gst.StateChangeReturn.NO_PREROLL:
        logger.warning("Pipeline might be missing a prerolled source.")
    else:
        logger.info("Pipeline is in PLAYING state successfully.")
    try:
        loop.run()
    except KeyboardInterrupt:
        pass

