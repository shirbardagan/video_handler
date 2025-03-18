import threading
from abc import abstractmethod, ABC

from aiortc.rtcdtlstransport import srtp_profile
from typing_extensions import List, Tuple

from app_instance import app
from common.base_logger import logger
import gi

from elements import NVH265DecWrapper, NVH264EncWrapper, X264enc
from elements.appsrc import VideoAppSrc
from elements.avdec_h265 import AVDecH265Wrapper
from elements.base_element_wrapper import GStreamerElementWrapper

gi.require_version('Gst', '1.0')
gi.require_version("GstApp", "1.0")
from gi.repository import Gst, GLib, GstApp


class BaseStreamPipeline(ABC):
    _lock = threading.Lock()

    def __init__(self):
        """Initializes the GStreamer pipeline and sets up the message bus."""
        self._instance = Gst.Pipeline.new("pipeline")
        self._elements = []
        self._bus = self._instance.get_bus()
        self._bus.add_watch(GLib.PRIORITY_DEFAULT, self.on_bus_message)

    @abstractmethod
    def create_pipeline(self) -> Gst.Pipeline:
        """
        Abstract method to be implemented by subclasses to define the pipeline structure.
        """
        pass

    @staticmethod
    def on_bus_message(bus: Gst.Bus, msg: Gst.Message) -> bool:
        # if msg.type == Gst.MessageType.ERROR:
        #     err, debug = msg.parse_error()
        #     logger.error(f"Error from {msg.src.get_name()}: {err.message} ({debug})")
        # elif msg.type == Gst.MessageType.WARNING:
        #     warn, debug = msg.parse_warning()
        #     logger.warning(f"Warning from {msg.src.get_name()}: {warn.message} ({debug})")
        # elif msg.type == Gst.MessageType.INFO:
        #     info, debug = msg.parse_info()
        #     logger.info(f"Info from {msg.src.get_name()}: {info.message} ({debug})")
        # elif msg.type == Gst.MessageType.STATE_CHANGED:
        #     old_state, new_state, pending = msg.parse_state_changed()
        #     logger.debug(f"{msg.src.get_name()} changed state from {old_state} to {new_state}")
        return True

    @staticmethod
    def has_elements_initialized(elements: List[GStreamerElementWrapper]) -> bool:
        """
        Checks if all elements in the pipeline are successfully initialized.

        Args:
            elements: List of GStreamer elements.
        Logs an error if any element is not initialized.
        """
        if any(e is None for e in elements):
            logger.error("Not all elements could be created.")
            return False
        return True

    @staticmethod
    def select_h265_decoder(use_gpu):
        try:
            if use_gpu:
                try:
                    return NVH265DecWrapper("nvh265decoder")
                except Exception as e:
                    logger.error("Failed initializing gpu h265 decoder", e)
                    return AVDecH265Wrapper("avh265decoder")
            else:
                return AVDecH265Wrapper("avh265decoder")
        except Exception as e:
            logger.error("While trying to determine gpu/cpu decoder: %s", e)

    @staticmethod
    def select_h264_encoder(use_gpu):
        try:
            if use_gpu:
                try:
                    return NVH264EncWrapper("nvh264encoder")
                except Exception as e:
                    logger.error("Failed initializing gpu h264 encoder", e)
                    return X264enc("x264encoder")
            else:
                return X264enc("x264encoder")
        except Exception as e:
            logger.error("While trying to determine gpu/cpu encoder: %s", e)

    @staticmethod
    def link_elements(links: List[Tuple[GStreamerElementWrapper, GStreamerElementWrapper]]) -> bool:
        """
        Links the provided GStreamer elements together.

        Args:
            links: A list of tuples where each tuple contains a source and a sink element.
        Logs an error if linking fails.
        """
        for src, sink in links:
            try:
                src.link(sink)
            except Exception as e:
                logger.error("While linking elements %s -> %s: %s", src.get_name(), sink.get_name(), e)
                return False
        return True

    def start_pipeline(self):
        ret = self._instance.set_state(Gst.State.PLAYING)

        if ret == Gst.StateChangeReturn.FAILURE:
            logger.error("Unable to set the MPEGPipeline to the playing state")
            return False
        else:
            logger.info("MPEGPipeline is now playing!!")
            return True

    def add_elements(self, elements: List[GStreamerElementWrapper]) -> None:
        """
        Adds elements to the GStreamer pipeline.

        Args:
            elements: List of elements to add to the pipeline.
        Logs an error if an element cannot be added.
        """
        for element in elements:
            try:
                self._instance.add(element.get_element())
                self._elements.append(element)
            except Exception as e:
                logger.error("While adding %s element to the pipeline: %s", element.get_name(), e)

    def unref(self) -> bool:
        """
        Releases pipeline resources and sets all elements to NULL state.

        If an element is an instance of VideoAppSrc and exists in `app.state.OPEN_CONNECTIONS`,
        it is removed from the open connections list before being released.
        Logs an error if resource cleanup fails.
        """
        with self._lock:
            try:
                self._instance.set_state(Gst.State.NULL)
                # self._instance.unref()
                for element in self._elements:
                    element.get_element().set_state(Gst.State.NULL)
                for element in self._elements:
                    if element.get_element().get_state(Gst.CLOCK_TIME_NONE)[1] == Gst.State.NULL:
                        element.get_element().unref()
                return True
            except Exception as e:
                logger.error("While releasing pipeline resources: %s", e)
                return False

    @staticmethod
    def get_pipeline_string(links: List[Tuple[GStreamerElementWrapper, GStreamerElementWrapper]]) -> str:
        """
        Generates a string representation of the GStreamer pipeline.

        Args:
            links: A list of tuples where each tuple contains a source and a sink element.
        Returns: The GStreamer pipeline string representation.
        """
        pipeline_parts = []

        for element1, element2 in links:
            pipeline_parts.append(element1.get_element_to_string())
            pipeline_parts.append("!")

        pipeline_parts.append(links[-1][1].get_element_to_string())
        return " ".join(pipeline_parts)

    def get_pipeline_elements(self) -> List[Gst.Element]:
        """Returns a list of the elements of the pipeline."""
        return self._elements
