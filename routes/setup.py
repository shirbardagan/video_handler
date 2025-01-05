from fastapi import APIRouter
from threading import Thread

from gi.repository import GObject, Gst

router = APIRouter()


@router.on_event("startup")
async def startup_event() -> None:
    print("On setup")
    Gst.init(None)
    thread_pipeline = Thread(target=GObject.MainLoop().run)
    thread_pipeline.start()
