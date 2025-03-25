import uvicorn

from fastapi import FastAPI

app = FastAPI(
    version="1.0.0",
    docs_url="/api-docs",
    swagger_ui_parameters={"useLocalStorage": True}
)


def start_server():
    uvicorn.run(app, host="0.0.0.0", port=8080)
