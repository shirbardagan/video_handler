from fastapi import APIRouter, Request
from fastapi.responses import FileResponse


router = APIRouter()


@router.get("/")
async def index():
    return FileResponse("static/index.html")
