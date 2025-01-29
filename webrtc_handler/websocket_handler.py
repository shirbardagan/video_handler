import asyncio
import json
import time

from common.base_logger import logger
import gi
from gi.repository import Gst, GObject, GstSdp

from pipelines.mp2t_h265_pipeline import MP2TH265StreamPipeline
from pipelines.webrtc_pipeline import WebRTCPipeline

gi.require_version('Gst', '1.0')
gi.require_version('GstWebRTC', '1.0')
gi.require_version('GstSdp', '1.0')


class WebRTCClient:
    def __init__(self, conn):
        self.conn = conn
        self.loop = asyncio.get_event_loop()

        webrtc_pipeline = MP2TH265StreamPipeline()
        self.pipeline = webrtc_pipeline.create_pipeline()
        self.webrtc = self.pipeline.get_by_name("webrtcbin")
        if self.webrtc is None:
            raise Exception("Failed to find webrtcbin element in the pipeline.")

        self.webrtc.connect('on-negotiation-needed', self.on_negotiation_needed)
        self.webrtc.connect('on-ice-candidate', self.send_ice_candidate_message)

    def start(self):
        try:
            logger.info("Starting pipeline")
            ret = self.pipeline.set_state(Gst.State.PLAYING)
            while self.pipeline.get_state(Gst.CLOCK_TIME_NONE)[1] != Gst.State.PLAYING:
                print("Not playing yet")

            if ret == Gst.StateChangeReturn.FAILURE:
                logger.error("Unable to set the pipeline to the playing state")
            else:
                print("Pipeline is now playing")
        except Exception as e:
            logger.error("While starting pipeline: %s", e)


    def send_sdp_offer(self, offer):
        text = offer.sdp.as_text()
        print(f"sending SDP offer")
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
        print("set local description")
        promise.interrupt()
        self.send_sdp_offer(offer)

    def on_negotiation_needed(self, element):
        self.webrtc.disconnect_by_func(self.on_negotiation_needed)
        promise = Gst.Promise.new_with_change_func(self.on_offer_created, None, None)
        print("Creating offer")
        element.emit('create-offer', None, promise)

    def send_ice_candidate_message(self, _, mlineindex, candidate):
        icemsg = json.dumps({'event': 'candidate', 'data':
            {
                'candidate': candidate,
                'sdpMLineIndex': mlineindex}
                             })
        print(f"Sending ICE candidate: {icemsg}")
        # print(self.webrtc.get_static_pad("sink_0").is_blocked())
        # print(self.webrtc.get_static_pad("sink_0").is_blocking())
        pad = self.webrtc.get_static_pad("sink_0")
        # pad.remove_probe(pad.get_current_probe_id())
        asyncio.run_coroutine_threadsafe(self.conn.send_text(icemsg), self.loop)
