import uvicorn

from fastapi import FastAPI

from config_models.config import SYSTEM_DEFAULT_HOST
from config_models.system_config import SystemSettingsConfig

app = FastAPI(
    version="1.0.0",
    docs_url="/api-docs",
    redoc_url=None,
    openapi_url="/openapi.json",
    swagger_ui_parameters={"useLocalStorage": True},
    swagger_ui_oauth2_redirect_url=None,
    swagger_ui_init_oauth={}
)
system_conf = SystemSettingsConfig()


def start_server():
    uvicorn.run(app, host=SYSTEM_DEFAULT_HOST, port=system_conf.port)
