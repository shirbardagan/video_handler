from fastapi import APIRouter
from threading import Thread
from gi.repository import GObject, Gst

from app_instance import app
from pipelines.webrtc_pipeline import WebRTCPipeline

router = APIRouter()


@router.on_event("startup")
async def startup_event() -> None:
    print("On setup")
    app.state.OPEN_CONNECTIONS = []
    app.state.CURRENT_PIPELINE = None
    app.state.STREAM = None
    Gst.init(None)

    thread_pipeline = Thread(target=GObject.MainLoop().run)
    thread_pipeline.start()
