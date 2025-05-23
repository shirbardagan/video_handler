import uvicorn

from fastapi import FastAPI

from config_models.config import SYSTEM_DEFAULT_HOST
from config_models.system_config import SystemSettingsConfig

app = FastAPI(
    version="1.0.0",
    docs_url="/api-docs",
    swagger_ui_parameters={"useLocalStorage": True}
)
system_conf = SystemSettingsConfig()

def start_server():
    uvicorn.run(app, host=SYSTEM_DEFAULT_HOST, port=system_conf.port)
