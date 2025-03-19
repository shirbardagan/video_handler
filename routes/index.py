from fastapi import APIRouter
from fastapi.responses import FileResponse
import os

router = APIRouter()

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
html_path = os.path.join(BASE_DIR, "..", "static", "index.html")
# html_path = os.getenv("HTML_PATH")

@router.get("/")
async def index():
    return FileResponse(html_path)

