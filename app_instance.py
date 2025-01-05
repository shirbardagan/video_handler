import uvicorn

from fastapi import FastAPI

app = FastAPI(
    version="1.0.0",
    docs_url="/api-docs"
)

app.state.PIPELINE_DATA = None


def start_server():
    uvicorn.run(app, host="127.0.0.1", port=8000)
