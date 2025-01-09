from fastapi import APIRouter
from fastapi.responses import FileResponse


router = APIRouter()

html_path = "/home/elbit/Downloads/video_handler/routes/index.html"

@router.get("/")
async def index():
    return FileResponse(html_path)

