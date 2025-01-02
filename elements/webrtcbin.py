import json
from gi.repository import Gst, GLib, GstApp
import asyncio

from app_instance import app
from common.base_logger import logger
from elements.base_element_wrapper import GStreamerElementWrapper


class WebRTCBinWrapper(GStreamerElementWrapper):
    def __init__(self, conn, name="webrtcbin"):
        super().__init__(name, "webrtcbin")
        self.conn = conn
        self.loop = asyncio.get_running_loop()

        self.element.connect('on-negotiation-needed', self.on_negotiation_needed)
        self.element.connect('on-ice-candidate', self.send_ice_candidate_message)

    def send_sdp_offer(self, offer):
        text = offer.sdp.as_text()
        logger.info(f"sending SDP offer")
        msg = json.dumps({'event': 'offer', 'data':
            {
                'type': 'offer',
                'sdp': text
            }
                          })
        loop = asyncio.new_event_loop()
        loop.run_until_complete(app.state.CONN.send_text(msg))

    def on_offer_created(self, promise, _, __):
        promise.wait()
        reply = promise.get_reply()
        offer = reply.get_value('offer')
        promise = Gst.Promise.new()
        self.get_element().emit('set-local-description', offer, promise)
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
        loop = asyncio.new_event_loop()
        loop.run_until_complete(self.conn.send_str(icemsg))
