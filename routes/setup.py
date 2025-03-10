import asyncio

from fastapi import APIRouter
from threading import Thread
from gi.repository import GObject, Gst

from app_instance import app
from pipelines.webrtc_pipeline import WebRTCPipeline

router = APIRouter()


@router.on_event("startup")
async def startup_event() -> None:
    print("On setup")
    Gst.init(None)

    app.state.OPEN_CONNECTIONS = []
    app.state.OPEN_CONNECTIONS_DATA = []
    app.state.CURRENT_PIPELINE = None
    app.state.CURR_PIPELINE = None
    app.state.STREAM = None
    app.state.channel = None
    app.state.event_loop = asyncio.get_running_loop()

    thread_pipeline = Thread(target=GObject.MainLoop().run)
    thread_pipeline.start()