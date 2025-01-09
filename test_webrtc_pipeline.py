import asyncio
import functools
import json
import threading

from gi.repository import Gst, GLib, GstApp, GObject, GstWebRTC

from common.base_logger import logger
import gi

from webrtc_handler.websocket_handler import WebRTCClient

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
    X264enc, AppSinkWrapper
)


def create_pipeline(webrtc_client, webrtcbin):
    pipeline = Gst.Pipeline.new("pipeline")
    initialized_pipeline_elements_tuple = (FileSrcWrapper("filesrc"),
                                           TSDemuxWrapper("tsdemux"),
                                           H265ParseWrapper("h265parse"),
                                           NVH265DecWrapper("nvh265dec"),
                                           X264enc("x264enc"),
                                           H264ParseWrapper("h264parse"),
                                           RTPH264Pay("rtph264pay"),
                                           AppSinkWrapper("appsink")
                                           )

    filesrc, tsdemux, h265parse, nvh265dec, x264enc, h264parse, rtph264pay, appsink = initialized_pipeline_elements_tuple

    if not all([filesrc.get_element(), tsdemux.get_element(), h265parse.get_element(), nvh265dec.get_element(),
                webrtcbin]):
        logger.error("Not all elements could be created.")

    filesrc.set_property("location", "/home/elbit/Desktop/flights/VNIR_ZOOM.ts")
    appsink.set_property("emit-signals", True)

    try:
        pipeline.add(filesrc.get_element())
        pipeline.add(tsdemux.get_element())
        pipeline.add(h265parse.get_element())
        pipeline.add(nvh265dec.get_element())
        pipeline.add(x264enc.get_element())
        pipeline.add(h264parse.get_element())
        pipeline.add(rtph264pay.get_element())
        pipeline.add(webrtcbin)
    except Exception as e:
        logger.error("While adding elements to the pipeline: %s", e)


    filesrc.link(tsdemux)
    tsdemux.connect("pad-added", functools.partial(tsdemux.on_pad_added, elements=h265parse.get_element()))
    h265parse.link(nvh265dec)
    nvh265dec.link(x264enc)
    x264enc.link(h264parse)
    h264parse.link(rtph264pay)
    rtph264pay.connect("pad-added", functools.partial(rtph264pay.on_pad_added, elements=webrtcbin))
    webrtcbin.connect('on-negotiation-needed', webrtc_client.on_negotiation_needed)
    webrtcbin.connect('on-ice-candidate', webrtc_client.send_ice_candidate_message)
    return pipeline

