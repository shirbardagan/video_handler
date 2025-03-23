import asyncio
import json

from app_instance import app
from common.base_logger import logger
import gi

gi.require_version('GstSdp', '1.0')
gi.require_version('GstWebRTC', '1.0')
gi.require_version('Gst', '1.0')
from gi.repository import Gst, GObject, GstSdp, GstWebRTC
from pipelines.webrtc_pipeline import WebRTCPipeline

gi.require_version('Gst', '1.0')
gi.require_version('GstWebRTC', '1.0')
gi.require_version('GstSdp', '1.0')


class WebRTCClient:
    """
    Manages the WebRTC pipeline for communication with a remote peer.
    It handles the setup of the pipeline, negotiation, sending SDP offers, and managing ICE candidates.

    Attributes:
        conn: The connection object used for sending messages to the remote peer.
        data_channel: The WebRTC data channel used for communication with the remote peer.
        webrtc_pipeline: The WebRTC pipeline object that facilitates media streaming.
        pipeline: The GStreamer pipeline that processes media data.
        webrtc: The webrtcbin element in the GStreamer pipeline that handles WebRTC communication.
    """
    def __init__(self, conn):
        self.data_channel = None
        self.conn = conn
        try:
            self.webrtc_pipeline = WebRTCPipeline()
            self.pipeline = self.webrtc_pipeline.create_pipeline()
            self.webrtc = self.pipeline.get_by_name("webrtcbin")
            self.videosrc = self.pipeline.get_by_name("videosrc")

            if self.webrtc is None:
                logger.error("webrtcbin element cannot be found in the pipeline.")
        except Exception as e:
            logger.error("While initializing WebRTCClient: %s", e)

        self.webrtc.connect('on-negotiation-needed', self.on_negotiation_needed)
        self.webrtc.connect('on-ice-candidate', self.send_ice_candidate_message)

    def start(self) -> bool:
        """
        Starts the GStreamer pipeline and sets it to the PLAYING state.

        Returns:
            bool: True if the pipeline starts successfully, False otherwise.
        """
        try:
            logger.info("Starting pipeline")
            ret = self.pipeline.set_state(Gst.State.PLAYING)
            if ret == Gst.StateChangeReturn.FAILURE:
                logger.error("Unable to set the pipeline to the playing state")
                return False
            else:
                logger.info("Pipeline is now playing")
                return True
        except Exception as e:
            logger.error("While starting pipeline: %s", e)
            return False

    def send_sdp_offer(self, offer: GstWebRTC.WebRTCSessionDescription) -> bool:
        """
        Sends an SDP offer to the remote peer.

        Args:
            offer: The SDP offer to be sent to the remote peer.
        """
        try:
            text = offer.sdp.as_text()
            logger.info(f"sending SDP offer")
            msg = json.dumps({'event': 'offer', 'data':
                {
                    'type': 'offer',
                    'sdp': text
                }
                              })
            asyncio.run_coroutine_threadsafe(self.conn.send_text(msg), app.state.event_loop)
            return True
        except Exception as e:
            logger.error("While sending SDP offer: %s", e)
            return False

    def on_offer_created(self, promise: Gst.Promise, _, __) -> None:
        """
        Callback function for when an offer is created. It sets the local description and sends the offer to the remote peer.
        Args:
            promise: The promise representing the asynchronous operation.
        """
        promise.wait()
        reply = promise.get_reply()
        offer = reply.get_value('offer')
        promise = Gst.Promise.new()
        self.webrtc.emit('set-local-description', offer, promise)
        logger.info("set local description")
        promise.interrupt()
        self.send_sdp_offer(offer)

    def on_data_channel_open(self, _) -> None:
        """Callback function for when the data channel is successfully opened."""
        logger.info("Data channel opened.")
        app.state.channels.append(self.data_channel)

    def on_negotiation_needed(self, element: Gst.Element) -> None:
        """
        Callback function for when negotiation is needed. It creates an offer and starts the data channel.
        Args:
            element: The GStreamer element (webrtcbin) that triggered the negotiation.
        """
        self.webrtc.disconnect_by_func(self.on_negotiation_needed)
        promise = Gst.Promise.new_with_change_func(self.on_offer_created, None, None)
        logger.info("Creating offer")
        element.emit('create-offer', None, promise)

        self.data_channel = self.webrtc.emit("create-data-channel", "klvdata", None)
        self.data_channel.connect("on-open", self.on_data_channel_open)


    def send_ice_candidate_message(self, _, mlineindex: int, candidate: str) -> bool:
        """
        Sends an ICE candidate to the remote peer.

        Args:
            mlineindex: The mline index of the ICE candidate.
            candidate: The ICE candidate to be sent to the remote peer.
        """
        try:
            icemsg = json.dumps({'event': 'candidate', 'data':
                {
                    'candidate': candidate,
                    'sdpMLineIndex': mlineindex}
                                 })
            logger.debug("Sending ICE candidate: %s", icemsg)
            asyncio.run_coroutine_threadsafe(self.conn.send_text(icemsg), app.state.event_loop)
        except Exception as e:
            logger.error("While sending ICE candidate: %s", e)
            return False