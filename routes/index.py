from fastapi import APIRouter
from fastapi.responses import FileResponse


router = APIRouter()

html_path = "/home/shir/PycharmProjects/video_handler/routes/index.html"

@router.get("/")
async def index():
    return FileResponse(html_path)

