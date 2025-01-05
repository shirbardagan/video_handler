import functools

import gi
from gi.repository import Gst, GLib, GstApp, GObject, GstWebRTC

from common.base_logger import logger
gi.require_version('GstWebRTC', '1.0')
gi.require_version('Gst', '1.0')

from elements import (
    FileSrcWrapper,
    TSDemuxWrapper,
    H265ParseWrapper,
    NVH265DecWrapper,
    VideoConvertWrapper,
    AutoVideoSinkWrapper
)

loop = GObject.MainLoop()
Gst.init(None)
pipeline = Gst.Pipeline.new("pipeline")
initialized_pipeline_elements_tuple = (FileSrcWrapper("filesrc"),
                                       TSDemuxWrapper("tsdemux"),
                                       H265ParseWrapper("h265parse"),
                                       NVH265DecWrapper("nvh265dec"),
                                       VideoConvertWrapper("videoconvert"),
                                       AutoVideoSinkWrapper("autovideosink"))

filesrc, tsdemux, h265parse, nvh265dec, videoconvert, autovideosink = initialized_pipeline_elements_tuple


if not all([filesrc.get_element(), tsdemux.get_element(), h265parse.get_element(), nvh265dec.get_element(),
            videoconvert.get_element(), autovideosink.get_element()]):
    logger.error("Not all elements could be created.")

filesrc.set_property("location", "/home/shir/Desktop/flights/VNIR_ZOOM.ts")
autovideosink.set_property("sync", False)

pipeline.add(filesrc.get_element())
pipeline.add(tsdemux.get_element())
pipeline.add(h265parse.get_element())
pipeline.add(nvh265dec.get_element())
pipeline.add(videoconvert.get_element())
pipeline.add(autovideosink.get_element())

filesrc.link(tsdemux)
tsdemux.connect("pad-added", functools.partial(tsdemux.on_pad_added, elements=h265parse.get_element()))
h265parse.link(nvh265dec)
nvh265dec.link(videoconvert)
videoconvert.link(autovideosink)

ret = pipeline.set_state(Gst.State.PLAYING)
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