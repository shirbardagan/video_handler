import uvicorn

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware


app = FastAPI(
    version="1.0.0",
    docs_url="/api-docs"
)

app.state.PIPELINE_DATA = None

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

def start_server():
    uvicorn.run(app, host="127.0.0.1", port=8000)