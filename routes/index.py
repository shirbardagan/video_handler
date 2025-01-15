from fastapi import APIRouter
from fastapi.responses import FileResponse
import os

router = APIRouter()

html_path = os.getenv("HTML_PATH")

@router.get("/")
async def index():
    return FileResponse(html_path)

