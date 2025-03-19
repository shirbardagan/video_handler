import asyncio

from fastapi import APIRouter
from threading import Thread
from gi.repository import GObject, Gst

from app_instance import app
from common.base_logger import logger
from pipelines.webrtc_pipeline import WebRTCPipeline

router = APIRouter()


@router.on_event("startup")
async def startup_event() -> None:
    logger.info("On setup.")
    if not Gst.is_initialized():
        Gst.init(None)

    app.state.request_data = None
    app.state.curr_pipeline = None
    app.state.curr_object = None

    app.state.webrtc_conn_videosrc = []
    app.state.channels = []
    app.state.conns = []
    app.state.event_loop = asyncio.get_running_loop()

    thread_pipeline = Thread(target=GObject.MainLoop().run)
    thread_pipeline.start()