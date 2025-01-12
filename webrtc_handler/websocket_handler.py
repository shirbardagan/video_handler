import asyncio
import functools
import json

from common.base_logger import logger
import gi
from gi.repository import Gst, GObject, GstSdp

from elements import WebRTCBinWrapper
from pipelines.mp2t_h265_pipeline import MP2TH265StreamPipeline

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

        pipe = MP2TH265StreamPipeline()
        self.pipeline = pipe.create_pipeline()
        self.webrtc = self.pipeline.get_by_name("webrtcbin")
        if self.webrtc is None:
            raise Exception("Failed to find webrtcbin element in the pipeline.")

        self.webrtc.connect('on-negotiation-needed', self.on_negotiation_needed)
        self.webrtc.connect('on-ice-candidate', self.send_ice_candidate_message)

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
        self.pipeline.set_state(Gst.State.PLAYING)
        logger.info("Playing pipeline")

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
        self.webrtc.emit('set-local-description', offer, promise)
        logger.info("set local description")
        promise.interrupt()
        self.send_sdp_offer(offer)

    def on_negotiation_needed(self, element):
        self.webrtc.disconnect_by_func(self.on_negotiation_needed)
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
