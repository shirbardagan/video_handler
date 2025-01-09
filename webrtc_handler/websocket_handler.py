import asyncio
import functools
import json

from common.base_logger import logger
import gi
from gi.repository import Gst, GObject, GstSdp

from elements import WebRTCBinWrapper

gi.require_version('Gst', '1.0')
gi.require_version('GstWebRTC', '1.0')
gi.require_version('GstSdp', '1.0')

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


class WebRTCClient:
    def __init__(self, conn):
        self.conn = conn
        self.loop = asyncio.get_running_loop()

        self.pipeline = self.create_pipeline()
        self.webrtc = self.pipeline.get_by_name("webrtcbin")
        if self.webrtc is None:
            raise Exception("Failed to find webrtcbin element in the pipeline.")

        # self.webrtc.connect('on-negotiation-needed', self.on_negotiation_needed)
        # self.webrtc.connect('on-ice-candidate', self.send_ice_candidate_message)

    def create_pipeline(self):
        pipeline = Gst.Pipeline.new("pipeline")
        initialized_pipeline_elements_tuple = (FileSrcWrapper("filesrc"),
                                               TSDemuxWrapper("tsdemux"),
                                               H265ParseWrapper("h265parse"),
                                               NVH265DecWrapper("nvh265dec"),
                                               X264enc("x264enc"),
                                               H264ParseWrapper("h264parse"),
                                               RTPH264Pay("rtph264pay"),
                                               WebRTCBinWrapper("webrtcbin"),
                                               )

        filesrc, tsdemux, h265parse, nvh265dec, x264enc, h264parse, rtph264pay, webrtcbin = initialized_pipeline_elements_tuple

        elements = [filesrc, tsdemux, h265parse, nvh265dec, x264enc, h264parse, rtph264pay, webrtcbin]

        if not all(elements):
            logger.error("Not all elements could be created.")

        filesrc.set_property("location", "/home/elbit/Desktop/flights/VNIR_ZOOM.ts")

        try:
            pipeline.add(filesrc.get_element())
            pipeline.add(tsdemux.get_element())
            pipeline.add(h265parse.get_element())
            pipeline.add(nvh265dec.get_element())
            pipeline.add(x264enc.get_element())
            pipeline.add(h264parse.get_element())
            pipeline.add(rtph264pay.get_element())
            pipeline.add(webrtcbin.get_element())
        except Exception as e:
            logger.error("While adding elements to the pipeline: %s", e)

        filesrc.link(tsdemux)
        tsdemux.get_element().connect("pad-added",
                                      functools.partial(tsdemux.on_pad_added, elements=h265parse.get_element()))
        h265parse.link(nvh265dec)
        nvh265dec.link(x264enc)
        x264enc.link(h264parse)
        h264parse.link(rtph264pay)
        webrtcbin_dynamic_sink_pad = webrtcbin.get_element().request_pad_simple("sink_%u")
        rtph264pay_src_pad = rtph264pay.get_element().get_static_pad("src")
        rtph264pay_src_pad.link(webrtcbin_dynamic_sink_pad)
        webrtcbin.get_element().connect('on-negotiation-needed', self.on_negotiation_needed)
        webrtcbin.get_element().connect('on-ice-candidate', self.send_ice_candidate_message)
        return pipeline


    def start(self):
        try:
            logger.info("Starting pipeline")
            ret = self.pipeline.set_state(Gst.State.PLAYING)
            if ret == Gst.StateChangeReturn.FAILURE:
                logger.error("Unable to set the pipeline to the playing state")
            else:
                logger.info("Pipeline is now playing")
        except Exception as e:
            logger.error("While starting pipeline: %s", e)


    def play(self):
        try:
            self.pipeline.set_state(Gst.State.PLAYING)
            logger.info("Playing pipeline")
        except Exception as e:
            logger.error("While playing pipeline: %s", e)


    def send_sdp_offer(self, offer):
        text = offer.sdp.as_text()
        logger.info(f"sending SDP offer")
        msg = json.dumps({'event': 'offer', 'data':
            {
                'type': 'offer',
                'sdp': text
            }
                          })
        asyncio.run_coroutine_threadsafe(self.conn.send_text(msg), self.loop)


    def on_offer_created(self, promise, _, __):
        promise.wait()
        reply = promise.get_reply()
        offer = reply.get_value('offer')
        promise = Gst.Promise.new()
        logger.info("set local description")
        promise.interrupt()
        self.send_sdp_offer(offer)


    def on_negotiation_needed(self, element):
        promise = Gst.Promise.new_with_change_func(self.on_offer_created, None, None)
        logger.info("Creating offer")
        element.emit('create-offer', None, promise)


    def send_ice_candidate_message(self, _, mlineindex, candidate):
        icemsg = json.dumps({'event': 'candidate', 'data':
            {
                'candidate': candidate,
                'sdpMLineIndex': mlineindex}
                             })
        asyncio.run_coroutine_threadsafe(self.conn.send_text(icemsg), self.loop)
