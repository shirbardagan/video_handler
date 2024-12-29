import uvicorn

from routes import command
from fastapi import FastAPI

app = FastAPI(
    version="1.0.0",
    docs_url="/api-docs"
)

app.include_router(command.router)

uvicorn.run(app, host="0.0.0.0", port=8000)