from fastapi import FastAPI
from fastapi.responses import JSONResponse

app = FastAPI()


@app.post("/api/pod/setliveness")
def set_liveness():
    """Check if the app is running."""
    return JSONResponse(content={"status": "true"}, status_code=200)


@app.get("/api/pod/getreadiness")
def get_readiness():
    """Check if the app is ready."""
    if app.state.readiness:
        return JSONResponse(content={"status": "true"}, status_code=200)
    return JSONResponse(content={"status": "false"}, status_code=503)

@app.get("/api/pod/getliveness")
def get_liveness():
    """Check if the app is ready."""
    if app.state.liveness:
        return JSONResponse(content={"status": "true"}, status_code=200)
    return JSONResponse(content={"status": "false"}, status_code=503)
