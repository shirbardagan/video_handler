import asyncio
import socket

from fastapi import APIRouter
from threading import Thread
from gi.repository import GObject, Gst

from app_instance import app
from common.base_logger import logger

router = APIRouter()


def get_host_ip() -> str:
    """Returns the local machine's IP."""
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        host_ip = s.getsockname()[0]
        return host_ip
    except socket.gaierror:
        logger.warning("Failed to retrieve local IP address, returning fallback 127.0.0.1.")
        return "127.0.0.1"


@router.on_event("startup")
async def startup_event() -> None:
    if not Gst.is_initialized():
        Gst.init(None)
    app.state.liveness = True
    app.state.readiness = True

    app.state.request_data = None
    app.state.curr_pipeline = None
    app.state.pipeline_status = False

    app.state.webrtc_conn_videosrc = []
    app.state.channels = []
    app.state.conns = []
    app.state.event_loop = asyncio.get_running_loop()
    app.state.host_ip = get_host_ip()

    mainloop = GObject.MainLoop()
    app.state.mainloop = mainloop
    thread_pipeline = Thread(target=mainloop.run, daemon=True)
    thread_pipeline.start()

    logger.debug("On setup. Application running on host %s.", app.state.host_ip)
