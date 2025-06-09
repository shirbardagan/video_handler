from fastapi import APIRouter
from fastapi.responses import JSONResponse

from app_instance import app
from models.bit_keepalive import Liveness
import logging

router = APIRouter()

logger = logging.getLogger(__name__)


@router.post("/api/pod/setliveness")
def set_liveness(data: Liveness):
    """Check if the app is running."""
    try:
        liveness_request = getattr(data, "liveness", None)
        if liveness_request:
            app.state.liveness = liveness_request
            return JSONResponse(content={"status": "true"}, status_code=200)
        else:
            return JSONResponse(content={"status": "false", "message": "Liveness value is empty."}, status_code=422)
    except Exception as e:
        logger.warning("Could not change liveness to false: %s", e)


@router.get("/api/pod/getreadiness")
def get_readiness():
    """Check if the app is ready."""
    if app.state.readiness:
        return JSONResponse(content={"status": "true"}, status_code=200)
    return JSONResponse(content={"status": "false"}, status_code=503)


@router.get("/api/pod/getliveness")
def get_liveness():
    """Check if the app is ready."""
    if app.state.liveness:
        return JSONResponse(content={"status": "true"}, status_code=200)
    return JSONResponse(content={"status": "false"}, status_code=503)
